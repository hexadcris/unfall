/*
    Copyright (C) 2012 Modelon AB <http://www.modelon.com>

	You should have received a copy of the LICENCE-FMUChecker.txt
    along with this program. If not, contact Modelon AB.
*/
/**
	\file fmi1_cs_sim.c
	Simulation loop for the FMI 1.0 Co-simulation FMUs
*/
#include <fmuChecker.h>
#include <fmilib.h>

jm_status_enu_t fmi1_cs_prep_init(fmu_check_data_t* cdata)
{	
	jm_status_enu_t jmstatus = jm_status_success;
	jm_callbacks* cb = &cdata->callbacks;

	fmi1_import_t* fmu = cdata->fmu1;
	fmi1_string_t fmuGUID = fmi1_import_get_GUID(fmu);
	fmi1_string_t mimeType;
	fmi1_real_t timeout = 0.0;
	fmi1_boolean_t visible = fmi1_false;
	fmi1_boolean_t interactive = fmi1_false;

	fmi1_real_t hstep;
    fmi1_real_t tend = fmi1_import_get_default_experiment_stop(fmu);

	mimeType = fmi1_import_get_mime_type(fmu);
	if(	(cdata->fmu1_kind == fmi1_fmu_kind_enu_cs_standalone) 
		|| !mimeType || !mimeType[0])
	{
		mimeType = "application/x-fmu-sharedlibrary";
	}
	else {
		if(	strcmp(mimeType, "application/x-fmu-sharedlibrary") != 0 )
		{
			jm_log_info(cb, fmu_checker_module,"The FMU requests simulator with MIME type '%s'.", mimeType);
			printf("\nPlease, start a simulator program for MIME type '%s'\nPress enter to continue.\n", mimeType);
			getchar();
		}
	}

    prepare_time_step_info(cdata, &tend, &hstep);

	cdata->instanceNameToCompare = "Test FMI 1.0 CS";
	cdata->instanceNameSavedPtr = 0;
	jmstatus = fmi1_import_instantiate_slave(fmu, cdata->instanceNameToCompare, 0, mimeType, timeout, visible, interactive);
	cdata->instanceNameSavedPtr = cdata->instanceNameToCompare;
	if (jmstatus == jm_status_error) {
		jm_log_fatal(cb, fmu_checker_module, "Could not instantiate the model");
		return jm_status_error;
	}

    return jmstatus;
}

jm_status_enu_t fmi1_cs_prep_simulate(fmu_check_data_t* cdata)
{
    fmi1_status_t fmistatus;
    fmi1_import_t* fmu = cdata->fmu1;
    jm_callbacks* cb = &cdata->callbacks;
    jm_status_enu_t jmstatus = jm_status_success;
    fmi1_boolean_t StopTimeDefined = fmi1_true;
    fmi1_real_t tstart = fmi1_import_get_default_experiment_start(fmu);
    fmi1_real_t tend = fmi1_import_get_default_experiment_stop(fmu);
    if (fmi1_status_ok_or_warning(fmistatus = check_fmi1_set_with_zero_len_array(fmu, cb)) &&
        fmi1_status_ok_or_warning(fmistatus = fmi1_import_initialize_slave(fmu, tstart, StopTimeDefined, tend)))
    {
        cdata->simulation_initialized = 1;
        jm_log_info(cb, fmu_checker_module, "Initialized FMU for simulation starting at time %g", tstart);
        if (fmi1_status_ok_or_warning(fmistatus = check_fmi1_get_with_zero_len_array(fmu, cb))) {
            fmistatus = fmi1_status_ok;
        } else {
            jmstatus = jm_status_error;
        }
    }
    else {
        jm_log_fatal(cb, fmu_checker_module, "Failed to initialize FMU for simulation (FMU status: %s)", fmi1_status_to_string(fmistatus));
        jmstatus = jm_status_error;
    }

	if(jmstatus != jm_status_error) {
		jm_log_verbose(cb, fmu_checker_module, "Writing simulation output for start time");
		if(fmi1_write_csv_data(cdata, tstart) != jm_status_success){
			jmstatus = jm_status_error;
		}
	}
    return jmstatus;
}

jm_status_enu_t fmi1_cs_simulate_step(fmu_check_data_t* cdata, fmi1_real_t tcur)
{

    fmi1_status_t       fmistatus           = fmi1_status_ok;
    jm_status_enu_t     jmstatus            = jm_status_success;
    jm_callbacks        *cb                 = &cdata->callbacks;

    fmi1_import_t       *fmu                = cdata->fmu1;
    fmi1_real_t         hstep               = cdata->stepSize;

    fmi1_boolean_t newStep = fmi1_true;
    fmi1_real_t tnext = tcur + hstep;


    jm_log_verbose(cb, fmu_checker_module, "Simulation step from time: %g until: %g", tcur, tnext);

    fmistatus = fmi1_import_do_step(fmu, tcur, hstep, newStep);

    tcur = tnext;

    if(fmi1_write_csv_data(cdata, tcur) != jm_status_success){
        jmstatus = jm_status_error;
    }

    if((fmistatus == fmi1_status_ok) || (fmistatus == fmi1_status_warning)) {
        fmistatus = fmi1_status_ok;
    }
    else
        jmstatus = jm_status_error;


	if((fmistatus != fmi1_status_ok) && (fmistatus != fmi1_status_warning)) {
		jm_log_fatal(cb, fmu_checker_module, "Simulation loop terminated at time %g since FMU returned status: %s", tcur, fmi1_status_to_string(fmistatus));
		jmstatus = jm_status_error;
    }

	return jmstatus;
}
