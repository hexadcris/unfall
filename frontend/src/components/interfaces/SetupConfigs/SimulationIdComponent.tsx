
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

import { Box, Typography, useTheme } from "@mui/material";
import { FilterCheckbox } from "../../../styles/general.styles";
interface Props {
    id: number;
    selected: boolean;
    handleChange: (id: number) => void;
    index: number;
}
const SimulationIdComponent = ({ id, selected, handleChange, index }: Props) => {
    const theme = useTheme();
    return (
        <Box
            data-testid={`test-simid-box-${id}`}
            display='flex'
            alignItems='center'
            gap={4}
            height='100%'
            sx={{ background: index % 2 === 0 ? theme.palette.common.white : `${theme.palette.primary.accent1}40`, ':last-of-type': { marginBottom: 30 }, ':first-of-type': {borderTop: `1px solid ${theme.palette.greyScale.main}`} , borderBottom: `1px solid ${theme.palette.greyScale.main}`, paddingLeft: 4, paddingY: 2, width: '100%' }}>
            <FilterCheckbox color="primary" sx={{ paddingRight: 6 }} data-testid={`test-simid-checkbox-${id}`} value={selected} checked={selected} onChange={() => handleChange(id)} />
            <Typography data-testid="test-id-label-empty-ids">
                {id}
            </Typography>
        </Box>
    )
}

export default SimulationIdComponent;