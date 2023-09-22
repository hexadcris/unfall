/*
 * Copyright (c) 2023 Hexad GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 */

import { useForm } from "../../../hooks/useForm";
import { IGlobalSetup } from "./globalSetup.d";
import { FooterComponent, SideWindowBase } from "../../../styles/general.styles";
import { GlobalSetupBox, GlobalSetupBoxTypography, GlobalSetupConfigBox, GlobalSetupTypography } from "../../../styles/globalSetup.styles";
import UploadFile from "../../commonComponents/UploadInput/UploadFile";
import { Button } from "@mui/material";

const GlobalSetup = () => {
  const { values } = useForm<IGlobalSetup>({pathToComponents: ''});
  const { pathToComponents } = values;
  return (
    <SideWindowBase>
      <GlobalSetupConfigBox data-testid="test-id-box-system-editor-section">
        <GlobalSetupTypography variant="h5">
          SYSTEM EDITOR
        </GlobalSetupTypography>
        <GlobalSetupBox>
          <GlobalSetupBoxTypography>
            Path to components
          </GlobalSetupBoxTypography>
          <UploadFile styles={{ flex: .8 }} route={pathToComponents} error={false} name="pathToComponents" handleFiles={() => { }} />
        </GlobalSetupBox>
      </GlobalSetupConfigBox>
      <GlobalSetupConfigBox>
        <GlobalSetupTypography variant="h5">
          PRE
        </GlobalSetupTypography>
        <GlobalSetupBox>
          <GlobalSetupBoxTypography>
            Path to End-to-End tests
          </GlobalSetupBoxTypography>
          <UploadFile styles={{ flex: .8 }} route="" error={false} name="" handleFiles={() => { }} />
        </GlobalSetupBox>
      </GlobalSetupConfigBox>
      <GlobalSetupConfigBox>
        <GlobalSetupTypography variant="h5">
          RUN
        </GlobalSetupTypography>
        <GlobalSetupBox>
          <GlobalSetupBoxTypography>
            Path to libraries/modules
          </GlobalSetupBoxTypography>
          <UploadFile styles={{ flex: .8 }} route="" error={false} name="" handleFiles={() => { }} />
        </GlobalSetupBox>
        <GlobalSetupBox>
          <GlobalSetupBoxTypography>
            Path to openPASS core
          </GlobalSetupBoxTypography>
          <UploadFile styles={{ flex: .8 }} route="" error={false} name="" handleFiles={() => { }} />
        </GlobalSetupBox>
        <GlobalSetupBox>
          <GlobalSetupBoxTypography>
            Path to openPASS simulation manager
          </GlobalSetupBoxTypography>
          <UploadFile styles={{ flex: .8 }} route="" error={false} name="" handleFiles={() => { }} />
        </GlobalSetupBox>
      </GlobalSetupConfigBox>
      <FooterComponent sx={{ justifyContent: 'flex-end' }} data-testid="test-id-footer-setup">
        <Button sx={{ marginRight: 6 }} color='primary' data-testid="test-id-button-saveChanges" variant='contained' name='saveChanges'>Save Changes</Button>
      </FooterComponent>
    </SideWindowBase>
  )
}

export default GlobalSetup;