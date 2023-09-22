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

import { Box, Button, Checkbox, Grid, Typography, useTheme } from "@mui/material"
import { InputStyled } from "../simulationManager.styles";
import UploadFile from "../../../commonComponents/UploadInput/UploadFile";
import { Simulation } from "../../../..";

interface Props {
    simName: string
    sim: Simulation
    index: number
    handleFiles: (name: string, path: string, index?: number) => void
    handleChange: (event: React.ChangeEvent<HTMLInputElement>, index: number) => void
    copySimulation: (simulation: Simulation) => void
    removeSimulation: (id: number) => void
}

const SimulationBox = ({ simName, sim, index, handleFiles, handleChange, copySimulation, removeSimulation }: Props) => {
    const theme = useTheme();
    return (
        <Box data-testid={`test-sim-${sim.id}`} sx={{ border: `1px solid ${theme.palette.greyScale.medium}`, marginX: theme.spacing(5), marginTop: theme.spacing(8), ':last-of-type': { marginBottom: 30 } }}>
            <Box sx={{ display: "flex", alignItems: "center", gap: '4px', paddingLeft: 5, borderBottom: `1px solid ${theme.palette.greyScale.medium}`, background: '#FFFFFF', paddingY: 1 }}>
                <Checkbox color="primary" data-testid={`test-id-checkbox-${sim.id}`} sx={{ width: 10 }} checked={sim.selected} name='selected' onChange={(event) => handleChange(event, index)} />
                <Typography style={{ fontWeight: 'bold', fontSize: '16px' }}> {simName} </Typography>
            </Box>
            <Box sx={{ display: 'flex', background: 'transparent' }}>
                <Grid container>
                    <Grid lg={9} md={8} xs={10} item sx={{ display: 'flex', flexDirection: 'column', paddingLeft: 4, paddingY: 4 }}>
                        <Box sx={{ display: 'inline-flex', alignItems: 'center', gap: 1, padding: 1, paddingRight: 4 }}>
                            <Typography sx={{ flex: .2 }}> Path to configs </Typography>
                            <UploadFile route={sim.configurations} error={sim.configurations === ''} styles={{ flex: 1 }} index={index} name='configurations' handleFiles={handleFiles} />
                        </Box>
                        <Box sx={{ display: 'inline-flex', alignItems: 'center', gap: 1, padding: 1, paddingRight: 4 }}>
                            <Typography sx={{ flex: .2 }}> Path to Results </Typography>
                            <UploadFile route={sim.results} error={sim.results === ''} styles={{ flex: 1 }} index={index} name='results' handleFiles={handleFiles} />
                        </Box>
                        <Box sx={{ display: 'inline-flex', alignItems: 'center', gap: 1, padding: 1, paddingRight: 4 }}>
                            <Typography sx={{ flex: .2 }}> Log File Simulation </Typography>
                            <UploadFile route={sim.logFileSimulation} error={sim.logFileSimulation === ''} styles={{ flex: 1 }} index={index} name='logFileSimulation' handleFiles={handleFiles} />
                        </Box>
                    </Grid>
                    <Grid sx={{ borderLeft: `1px solid ${theme.palette.greyScale.medium}`, display: 'flex', flexDirection: 'column', alignItems: 'center', justifyContent: 'space-evenly' }} lg={3} md={4} xs={2} item>
                        <Box sx={{ display: 'flex', alignItems: 'center' }}>
                            <Typography >Number of runs: </Typography>
                            <InputStyled sx={{ marginLeft: 2, marginRight: 1 }}
                                value={sim.numberOfExecutions}
                                size="small"
                                name='numberOfExecutions'
                                id={`numberOfExecutions-${index}`}
                                onChange={(event: React.ChangeEvent<HTMLInputElement>) => handleChange(event, index)}
                                inputProps={{
                                    step: 1,
                                    min: 0,
                                    max: 100,
                                    type: 'number',
                                }}
                            />
                        </Box>
                        <Box sx={{ display: 'flex', justifyContent: 'center', gap: 1 }}>
                            <Button data-testid={`test-id-simbutton-duplicate-${sim.id}`} color='primary' variant='outlined' onClick={() => copySimulation(sim)}>Duplicate</Button>
                            <Button data-testid={`test-id-simbutton-delete-${sim.id}`} color='primary' variant='contained' onClick={() => removeSimulation(sim.id)}>Delete</Button>
                        </Box>
                    </Grid>
                </Grid>
            </Box>
        </Box>
    )
}

export default SimulationBox;