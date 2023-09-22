/*
Copyright (C) 2012 Modelon AB <http://www.modelon.com>

You should have received a copy of the LICENCE-FMUChecker.txt
along with this program. If not, contact Modelon AB.
*/
/**
\file fmi1_check.c
Driver for FMI 1.0 checking and IO routines.
*/

#include <errno.h>
#include <assert.h>

#include <fmuChecker.h>
#include <fmilib.h>

void  fmi1_checker_logger(fmi1_component_t c, fmi1_string_t instanceName, fmi1_status_t status, fmi1_string_t category, fmi1_string_t message, ...){

	fmu_check_data_t* cdata = cdata_global_ptr;
	fmi1_import_t* fmu = cdata->fmu1;
	jm_callbacks* cb = &cdata->callbacks;
	jm_log_level_enu_t logLevel;
	char buf[100000], *curp = buf;
	const char* statusStr;
	va_list args;

	assert(cdata);
	assert(fmu);

	if(!cdata->printed_instance_name_error_flg) {
		if(strcmp(instanceName, cdata->instanceNameToCompare) != 0) {
			jm_log_error(cb, fmu_checker_module, "FMU does not utilize provided instance name (%s != %s)", cdata->instanceNameToCompare, instanceName);
			cdata->printed_instance_name_error_flg = 1;
		}
		else if(instanceName == cdata->instanceNameSavedPtr) {
			jm_log_error(cb, fmu_checker_module, "FMU does not make an internal copy of provided instance name (violation of fmiString handling)",
				cdata->instanceNameToCompare, instanceName);
			cdata->printed_instance_name_error_flg = 1;
		}
	}

	switch(status) {
	case fmi1_status_pending:
	case fmi1_status_ok:
		logLevel = jm_log_level_verbose;
		break;
	case fmi1_status_discard:
	case fmi1_status_warning:
		logLevel = jm_log_level_warning;
		break;
	case fmi1_status_error:
		logLevel = jm_log_level_error;
		break;
	case fmi1_status_fatal:
	default:
		logLevel = jm_log_level_fatal;
	}

	if(logLevel > cb->log_level) return;
	if(logLevel < jm_log_level_info)
		cdata->num_fmu_messages++;

	if(category && *category) {
		sprintf(curp, "\t[FMU][%s]", category);
	}
	else {
		sprintf(curp, "\t[FMU]");
	}
	curp += strlen(curp);
	statusStr = fmi1_status_to_string(status);
	sprintf(curp, "[FMU status:%s] ", statusStr);
	curp += strlen(statusStr) + strlen("[FMU status:] ");
	va_start (args, message);
	vsprintf(curp, message, args);
	fmi1_import_expand_variable_references(fmu, buf, cb->errMessageBuffer,JM_MAX_ERROR_MESSAGE_SIZE);
	va_end (args);
	checker_logger(cb, fmu_checker_module, jm_log_level_nothing, cb->errMessageBuffer);
}

int fmi1_filter_outputs(fmi1_import_variable_t*vl, void * data) {
	return (fmi1_import_get_causality(vl) == fmi1_causality_enu_output);
}

jm_status_enu_t fmi1_check(fmu_check_data_t* cdata) {
	fmi1_callback_functions_t callBackFunctions;
	jm_callbacks* cb = &cdata->callbacks;
	jm_status_enu_t status = jm_status_success;

	cdata->fmu1 = fmi1_import_parse_xml(cdata->context, cdata->tmpPath);

	if(!cdata->fmu1) {
		jm_log_fatal(cb,fmu_checker_module,"Error parsing XML, exiting");
		return jm_status_error;
	}

	cdata->modelIdentifierFMI1 = fmi1_import_get_model_identifier(cdata->fmu1);
	cdata->modelName = fmi1_import_get_model_name(cdata->fmu1);
	cdata->GUID = fmi1_import_get_GUID(cdata->fmu1);

	jm_log_info(cb, fmu_checker_module,"Model name: %s", cdata->modelName);
	jm_log_info(cb, fmu_checker_module,"Model identifier: %s", cdata->modelIdentifierFMI1);
	jm_log_info(cb, fmu_checker_module,"Model GUID: %s", cdata->GUID);
	jm_log_info(cb, fmu_checker_module,"Model version: %s", fmi1_import_get_model_version(cdata->fmu1));

	cdata->fmu1_kind = fmi1_import_get_fmu_kind(cdata->fmu1);

	jm_log_info(cb, fmu_checker_module,"FMU kind: %s", fmi1_fmu_kind_to_string(cdata->fmu1_kind));

	cdata->vl = fmi1_import_get_variable_list(cdata->fmu1);

	if(!cdata->vl) {
		jm_log_fatal(cb, fmu_checker_module,"Could not construct model variables list");
		return jm_status_error;
	}

	if(cb->log_level >= jm_log_level_info) {
		char buf[10000];
		fmi1_import_model_counts_t counts;
		fmi1_import_collect_model_counts(cdata->fmu1, &counts);
		sprintf(buf, 
			"The FMU contains:\n"
			"%u constants\n"
			"%u parameters\n"
			"%u discrete variables\n"
			"%u continuous variables\n"
			"%u inputs\n"
			"%u outputs\n"
			"%u internal variables\n"
			"%u variables with causality 'none'\n"
			"%u real variables\n"
			"%u integer variables\n"
			"%u enumeration variables\n"
			"%u boolean variables\n"
			"%u string variables\n",
			counts.num_constants,
			counts.num_parameters,
			counts.num_discrete,
			counts.num_continuous,
			counts.num_inputs,
			counts.num_outputs,
			counts.num_internal,
			counts.num_causality_none,
			counts.num_real_vars,
			counts.num_integer_vars,
			counts.num_enum_vars,
			counts.num_bool_vars,
			counts.num_string_vars);

		checker_logger(cb, fmu_checker_module,jm_log_level_info,buf);

		if (!cdata->inputFileName && (counts.num_inputs>0)){
			jm_log_info(cb, fmu_checker_module,"No input data provided. In case of simulation initial values from FMU will be used.");
		}
	}

	jm_log_info(cb, fmu_checker_module,"Printing output file header");
	if(fmi1_write_csv_header(cdata) != jm_status_success) {
		return jm_status_error;
	}

	if(!cdata->do_simulate_flg) {
		jm_log_verbose(cb, fmu_checker_module,"Simulation was not requested");
		return jm_status_success;
	}

	callBackFunctions.allocateMemory = check_calloc;
	callBackFunctions.freeMemory = check_free;
	callBackFunctions.logger = fmi1_checker_logger;
	callBackFunctions.stepFinished = 0;

	if ((cdata->fmu1_kind > fmi1_fmu_kind_enu_me) && cdata->require_me) {
		jm_log_error(cb, fmu_checker_module, "Testing of ME requested but not an ME FMU!");
	}
	if( (cdata->fmu1_kind == fmi1_fmu_kind_enu_me) && (cdata->do_test_me) ) {

		status = fmi1_import_create_dllfmu(cdata->fmu1, callBackFunctions, 0);

		if (status == jm_status_error) {
			jm_log_fatal(cb,fmu_checker_module,"Could not create the DLL loading mechanism(C-API).");
			return jm_status_error;
		}
		else {
            /* NOTE: setting debug mode prevents unloading of shared object during shutdown.
             * This leads to segmentation faults when loading the same FMU multiple times.
			if(cdata->tmpPath == cdata->unzipPath) {
                fmi1_import_set_debug_mode(cdata->fmu1, 1);
            }
            */
			jm_log_info(cb,fmu_checker_module,"Version returned from FMU:   %s", fmi1_import_get_version(cdata->fmu1));

			{
				const char* platform;

				platform= fmi1_import_get_model_types_platform(cdata->fmu1);
				
				if(strcmp(platform, fmi1_get_platform())) 
					jm_log_error(cb,fmu_checker_module,"Platform type returned from FMU %s does not match the checker  %s",platform, fmi1_get_platform() );
			}

			status = fmi1_me_simulate(cdata);
		}
	}
	if ((cdata->fmu1_kind & ((cdata->fmu1_kind == fmi1_fmu_kind_enu_cs_standalone ) || (cdata->fmu1_kind == fmi1_fmu_kind_enu_cs_tool ))) == 0 && cdata->require_cs) {
		jm_log_error(cb, fmu_checker_module, "Testing of CS requested but not a CS FMU!");
	}
	if( ((cdata->fmu1_kind == fmi1_fmu_kind_enu_cs_standalone ) || (cdata->fmu1_kind == fmi1_fmu_kind_enu_cs_tool ))
		&& cdata->do_test_cs) {
			jm_status_enu_t savedStatus = status;

			status = fmi1_import_create_dllfmu(cdata->fmu1, callBackFunctions, 0);

			if (status == jm_status_error) {
				jm_log_fatal(cb,fmu_checker_module,"Could not create the DLL loading mechanism(C-API) for CoSimulation.");
			}
			else {
                /* NOTE: setting debug mode prevents unloading of shared object during shutdown.
                 * This leads to segmentation faults when loading the same FMU multiple times.
                if(cdata->tmpPath == cdata->unzipPath) {
                    fmi1_import_set_debug_mode(cdata->fmu1, 1);
				}
                */
				jm_log_info(cb,fmu_checker_module,"Version returned from FMU:   %s", fmi1_import_get_version(cdata->fmu1));

				{
					const char* platform;

					platform= fmi1_import_get_types_platform(cdata->fmu1);

					if(strcmp(platform, fmi1_get_platform())) 
						jm_log_error(cb,fmu_checker_module,"Platform type returned from FMU %s does not match the checker  %s",platform, fmi1_get_platform() );
				}
			}
			if(status == jm_status_success) status = savedStatus;
			else if((status == jm_status_warning) && (savedStatus == jm_status_error)) status = jm_status_error;
	}
	if( (cdata->fmu1_kind == fmi1_fmu_kind_enu_unknown)){
		jm_log_error(cb, fmu_checker_module, "Could not determine FMU kind. No simulation.");
		return jm_status_error;
	}
	return status;
}


jm_status_enu_t fmi1_write_csv_header(fmu_check_data_t* cdata) {
	fmi1_import_variable_list_t * vl = cdata->vl;
	unsigned i, n = (unsigned)fmi1_import_get_variable_list_size(vl);

	char replace_sep = ':';

	if(cdata->CSV_separator == ':') {
		replace_sep = '|';
	}

	if (cdata->do_mangle_var_names){
		if(checked_fprintf(cdata,"time") != jm_status_success) {
			return jm_status_error;	
		}
	}
	else {
		if(checked_fprintf(cdata,"\"time\"") != jm_status_success) {
			return jm_status_error;
		}
	}

	for(i = 0; i < n; i++) {
		/*		char buf[10000], *cursrc, *curdest;
		int need_quoting = 0; */
		fmi1_import_variable_t * v = fmi1_import_get_variable(vl, i);
		const char* vn = fmi1_import_get_variable_name(v);
		fmi1_import_variable_t * vb = fmi1_import_get_variable_alias_base(cdata->fmu1, v);
		jm_status_enu_t status = jm_status_success;
#if 0
		/* handlinf of aliases is switched off for now */
		switch(fmi1_import_get_variable_alias_kind(v)) {
		case fmi1_variable_is_not_alias:
			sprintf(buf, "%s", vn);
			break;
		case fmi1_variable_is_negated_alias: {
			fmi1_base_type_enu_t btype = fmi1_import_get_variable_base_type(v);
			sprintf(buf, "%s=-%s", 	vn, fmi1_import_get_variable_name(vb));
			if( btype == fmi1_base_type_enum) {
				jm_log_error(&cdata->callbacks, fmu_checker_module, "Negated alias is not meaningful for enum variable %s. Ignoring.", vn);
			}
			else if( btype == fmi1_base_type_str) {
				jm_log_error(&cdata->callbacks, fmu_checker_module, "Negated alias is not meaningful for string variable %s. Ignoring.", vn);
			}
			break;
											 }
		case fmi1_variable_is_alias:
			sprintf(buf, "%s=%s", vn, fmi1_import_get_variable_name(vb));
			break;
		default:
			assert(0);
		}
#endif
		if (cdata->do_output_all_vars || (fmi1_import_get_causality(v) == fmi1_causality_enu_output)){
			status = check_fprintf_var_name(cdata, vn);
		}
		if(status != jm_status_success) {
			return jm_status_error;
		}
	}

	if(checked_fprintf(cdata, "\r\n") != jm_status_success) {
		return jm_status_error;
	}
	return jm_status_success;
}

jm_status_enu_t fmi1_write_csv_data(fmu_check_data_t* cdata, double time) {
	fmi1_import_t* fmu = cdata->fmu1;
	fmi1_import_variable_list_t * vl = cdata->vl;
	jm_callbacks* cb = &cdata->callbacks;
	fmi1_status_t fmistatus = fmi1_status_ok;
	jm_status_enu_t outstatus = jm_status_success;
	unsigned i, n = (unsigned)fmi1_import_get_variable_list_size(vl);

	char fmt_sep[2];
	char fmt_r[20];
	char fmt_i[20];
	char fmt_true[20];
	char fmt_false[20];

	if(cdata->maxOutputPts > 0) {
		if(time < cdata->nextOutputTime) {
			return jm_status_success;
		}
		else {
			cdata->nextOutputStep++;
			cdata->nextOutputTime = cdata->stopTime*cdata->nextOutputStep/cdata->maxOutputPts;
			if(cdata->nextOutputTime > cdata->stopTime) {
				cdata->nextOutputTime = cdata->stopTime;
			}
		}
	}

	fmt_sep[0] = cdata->CSV_separator; fmt_sep[1] = 0;
	sprintf(fmt_r, "%c%s", cdata->CSV_separator, "%.16E");
	sprintf(fmt_i, "%c%s", cdata->CSV_separator, "%d");
#ifdef SUPPORT_out_enum_as_int_flag
	if(!cdata->out_enum_as_int_flag) {
		sprintf(fmt_true, "%ctrue", cdata->CSV_separator);
		sprintf(fmt_false, "%cfalse", cdata->CSV_separator);
	}
	else
#endif
	{
		sprintf(fmt_true, "%c1", cdata->CSV_separator);
		sprintf(fmt_false, "%c0", cdata->CSV_separator);
	}

	if(checked_fprintf(cdata, "%.16E", time) != jm_status_success) {
		return jm_status_error;
	}

	for(i = 0; i < n; i++) {
		fmi1_import_variable_t* v = fmi1_import_get_variable(vl, i);
		fmi1_value_reference_t vr = fmi1_import_get_variable_vr(v); 
		switch(fmi1_import_get_variable_base_type(v)) {
		case fmi1_base_type_real:
			{
				double val;
				fmistatus = fmi1_import_get_real(fmu,&vr, 1, &val);
				if (cdata->do_output_all_vars || (fmi1_import_get_causality(v) == fmi1_causality_enu_output)){
					if(fmi1_import_get_variable_alias_kind(v) == fmi1_variable_is_negated_alias)
						val = -val;
					outstatus = checked_fprintf(cdata, fmt_r, val);
				}
				break;
			}
		case fmi1_base_type_int:
			{
				int val;
				fmistatus = fmi1_import_get_integer(fmu,&vr, 1, &val);
				if (cdata->do_output_all_vars || (fmi1_import_get_causality(v) == fmi1_causality_enu_output)){
					if(fmi1_import_get_variable_alias_kind(v) == fmi1_variable_is_negated_alias)
						val = -val;
					outstatus = checked_fprintf(cdata, fmt_i, val);
				}
				break;
			}
		case fmi1_base_type_bool:
			{
				fmi1_boolean_t val;
				char* fmt;
				fmistatus = fmi1_import_get_boolean(fmu,&vr, 1, &val);
				if (cdata->do_output_all_vars || (fmi1_import_get_causality(v) == fmi1_causality_enu_output)){
					if(fmi1_import_get_variable_alias_kind(v) == fmi1_variable_is_negated_alias)
						fmt = (val == fmi1_false) ? fmt_true:fmt_false;
					else
						fmt = (val == fmi1_true) ? fmt_true:fmt_false;
					outstatus = checked_fprintf(cdata, fmt);
				}
				break;
			}
		case fmi1_base_type_str:
			{
				char empty = 0;
				fmi1_string_t val = "test ##";

				fmistatus = fmi1_import_get_string(fmu,&vr, 1, &val);
				if (cdata->do_output_all_vars || (fmi1_import_get_causality(v) == fmi1_causality_enu_output)){
					checked_fprintf(cdata, fmt_sep);
					outstatus = checked_print_quoted_str(cdata, val);
				}
				break;
			}
		case fmi1_base_type_enum:
			{
				int val;
				fmi1_import_variable_typedef_t* t = fmi1_import_get_variable_declared_type(v);
				fmi1_import_enumeration_typedef_t* et = 0;
				unsigned int item = 0;
				const char* itname = 0;
				if(t) et = fmi1_import_get_type_as_enum(t);

				fmistatus = fmi1_import_get_integer(fmu,&vr, 1, &val);
				if(et) itname = fmi1_import_get_enum_type_item_name(et, val);
				if(!itname) {
					jm_log_error(cb, fmu_checker_module, "Could not get item name for enum variable %s", fmi1_import_get_variable_name(v));
				}
#ifdef SUPPORT_out_enum_as_int_flag
				if(!cdata->out_enum_as_int_flag && itname) {
					checked_fprintf(cdata, fmt_sep);
					outstatus = checked_print_quoted_str(cdata, itname);					
				}
				else 
#endif
				{
					if (cdata->do_output_all_vars || (fmi1_import_get_causality(v) == fmi1_causality_enu_output)){
						outstatus = checked_fprintf(cdata, fmt_i, val);
					}
				}
				break;
			}
		}
		if(  fmistatus != fmi1_status_ok) {
			jm_log_warning(cb, fmu_checker_module, "fmiGetXXX returned status: %s for variable %s", 
				fmi1_status_to_string(fmistatus), fmi1_import_get_variable_name(v));
		}

		if(outstatus != jm_status_success) {
			return jm_status_error;
		}
	}
	if(checked_fprintf(cdata, "\r\n")!= jm_status_success) {
		return jm_status_error;
	}
	return jm_status_success;
}

fmi1_status_t check_fmi1_get_with_zero_len_array(fmi1_import_t* fmu, jm_callbacks* cb)
{
    fmi1_status_t fmistatus;
    jm_log_info(cb, fmu_checker_module, "Checking that FMI get functions can be called with zero length arrays");
    if (fmi1_status_ok_or_warning(fmistatus = fmi1_import_set_real(fmu, NULL, 0, 0)) &&
        fmi1_status_ok_or_warning(fmistatus = fmi1_import_set_integer(fmu, NULL, 0, 0)) &&
        fmi1_status_ok_or_warning(fmistatus = fmi1_import_set_boolean(fmu, NULL, 0, 0)) &&
        fmi1_status_ok_or_warning(fmistatus = fmi1_import_set_string(fmu, NULL, 0, 0)))
    {
        return fmistatus;
    }

    jm_log_fatal(cb, fmu_checker_module, "Calling FMI get functions with zero length arrays failed");
    return fmistatus;
}

fmi1_status_t check_fmi1_set_with_zero_len_array(fmi1_import_t* fmu, jm_callbacks* cb)
{
    fmi1_status_t fmistatus;
    jm_log_info(cb, fmu_checker_module, "Checking that FMI set functions can be called with zero length arrays");
    if (fmi1_status_ok_or_warning(fmistatus = fmi1_import_set_real(fmu, NULL, 0, 0)) &&
        fmi1_status_ok_or_warning(fmistatus = fmi1_import_set_integer(fmu, NULL, 0, 0)) &&
        fmi1_status_ok_or_warning(fmistatus = fmi1_import_set_boolean(fmu, NULL, 0, 0)) &&
        fmi1_status_ok_or_warning(fmistatus = fmi1_import_set_string(fmu, NULL, 0, 0)))
    {
        return fmistatus;
    }

    jm_log_fatal(cb, fmu_checker_module, "Calling FMI set functions with zero length arrays failed");
    return fmistatus;
}
