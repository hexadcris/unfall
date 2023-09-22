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

import { SideWindowBase } from "../../../styles/general.styles"
import { Typography } from "@mui/material"
import { HomeBox, HomeButton, HomeContainer } from "../../../styles/home.styles";
import { PATHS } from "../../../index.d";
import { useNavigate } from "react-router";
import { useGlobalContext } from "../../../context/ContextProvider";

const Home = () => {
  const { setSelectedTab } = useGlobalContext();
  const navigation = useNavigate();
  const handleClick = (path: PATHS) => {
    navigation(path);
    setSelectedTab(path);
  }
  return (
    <SideWindowBase className="home" data-testid="test-id-interface-home">
      <HomeContainer>
        <HomeBox data-testid="test-id-simulationbox-home">
          <Typography>
            Run the simulations
          </Typography>
          <HomeButton data-testid="test-id-button-simulationbox-home" onClick={() => handleClick(PATHS.SIMULATION_MANAGER)} variant="contained">Run</HomeButton>
        </HomeBox>
        <HomeBox data-testid="test-id-reportbox-home">
          <Typography>
            Simulation Report
          </Typography>
          <HomeButton data-testid="test-id-button-reportbox-home" data-type="monitor" onClick={() => handleClick(PATHS.SIMULATION_REPORT)} variant="contained">Monitor</HomeButton>
        </HomeBox>
        <HomeBox data-testid="test-id-preparebox-home">
          <Typography>
            Prepare the simulations configurations
          </Typography>
          <HomeButton data-testid="test-id-button-preparebox-home" onClick={() => handleClick(PATHS.SETUP_CONFIGS)} variant="contained">Pre</HomeButton>
        </HomeBox>
      </HomeContainer>
    </SideWindowBase>
  )
}

export default Home