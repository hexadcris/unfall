import { Box, Grid, Typography, Checkbox, useTheme } from "@mui/material";
import UploadFile from "../../commonComponents/UploadInput/UploadFile";
import { IAgent, ISetupConfig } from "./setupConfigs";
import { verifyPath } from "../../../utils/api.util";
import { useGlobalContext } from "../../../context/ContextProvider";
import { INPUT_FILE } from "../../../index.d";

interface Props extends IAgent {
    index: number;
    handleChange: (newValues: Partial<ISetupConfig>) => void;
    agents: IAgent[]
}
const AgentComponent = ({ name, file, index, selected, handleChange, agents }: Props) => {
    const theme = useTheme();
    const { modal, setModal } = useGlobalContext();
    const handleChangeAgents = (event: React.ChangeEvent<HTMLInputElement>, index: number) => {
        const newAgents = [...agents];
        newAgents[index].selected = event.target.checked;
        handleChange({ agents: newAgents });
    }
    const handleFiles = async (_: string, path: string, index?: number) => {
        const response = await verifyPath(path);
        if (!response?.ok) {
            setModal({
                ...modal,
                active: true,
                loadingBar: false,
                title: 'Path Not Found',
                icon: null,
                descriptionTitle: '',
                description: response?.realPath || '',
                buttons: [{
                    title: 'Close',
                }]
            });
            return;
        }
        const newAgents = [...agents];
        newAgents[index!].file = response.realPath || '';
        handleChange({ agents: newAgents });
    }
    return (
        <Grid container sx={{ background: index % 2 === 0 ? theme.palette.common.white : `${theme.palette.primary.accent1}20`, borderBottom: `1px solid ${theme.palette.greyScale.main}`, ':last-of-type': { borderBottom: 'unset' }, paddingX: 4, paddingY: 2, width: '100%' }}>
            <Grid lg={4} md={4} sm={4} item sx={{ display: 'flex', flexDirection: 'column' }}>
                <Box display='flex' flex={1} gap={2} alignItems='center'>
                    <Checkbox data-testid={`test-id-checkbox-agent-${name}`} color="primary" checked={selected} onChange={(e) => handleChangeAgents(e, index)} />
                    <Typography>
                        {name}
                    </Typography>
                </Box>
            </Grid>
            <Grid lg={8} md={8} sm={8} item sx={{ display: 'flex', flexDirection: 'column' }}>
                <Box display='flex' flex={1} gap={2}>
                    <Box marginLeft={4} sx={{ display: 'flex', alignItems: 'center', width: '95%' }}>
                        <UploadFile
                            route={file}
                            error={(selected && file === '')}
                            styles={{ flex: 1 }}
                            name={`agentFile-${index}`}
                            index={index}
                            handleFiles={handleFiles}
                            type={INPUT_FILE.file}
                        />
                    </Box>
                </Box>
            </Grid>
        </Grid>
    )
}

export default AgentComponent;