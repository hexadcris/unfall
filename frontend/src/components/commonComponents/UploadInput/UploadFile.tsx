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

import { Button, } from '@mui/material';
import { useRef } from 'react';
import { IGenericRecord, INPUT_FILE, Variant } from '../../../index.d';
import { DivGrouped, InputFile } from '../../../styles/general.styles';
import { useGlobalContext } from '../../../context/ContextProvider';
import { deleteInformation, verifyPath } from '../../../utils/api.util';

interface Props {
    route: string;
    handleFiles: (name: string, path: string, index?: number) => void;
    index?: number;
    name: string;
    styles?: IGenericRecord;
    error: boolean;
    type?: INPUT_FILE;
    buttonDisabled?: boolean;
    handleCustomFile?: (file: File, inputName: string) => void;
    accept?: string;
}

const UploadFile = ({ route, handleFiles, index, name, styles, error, type = INPUT_FILE.dir, buttonDisabled, handleCustomFile, accept = "application/xml" }: Props) => {
    const { modal, setModal } = useGlobalContext();
    const fileRef = useRef<HTMLInputElement>(null);
    const deleteDataFolder = (path: string) => {
        setModal({
            ...modal,
            active: true,
            descriptionTitle: "",
            title: 'Data Found On Folder',
            description: `Information was found inside ${path} folder, would you like to keep it or delete it?.`,
            buttons: [
                {
                    title: 'Keep It',
                    action: () => handleFiles(name, path, index!),
                    variant: Variant.outlined,
                    testId: 'test-id-modalbutton-keepit'
                },
                {
                    title: 'Delete Information',
                    action: async () => {
                        await deleteInformation(path);
                        handleFiles(name, path, index!);
                    },
                    testId: 'test-id-modalbutton-delete-information'
                },
            ],
        });
    }

    const handleChange = async (event: React.ChangeEvent<HTMLInputElement>) => {
        const { files } = event.target;
        if (handleCustomFile) {
            handleCustomFile(files![0], name);
            return;
        }
        handleFiles(name, files![0].name, index);
    };

    const handleClick = async () => {
        try {
            if (type === INPUT_FILE.file) {
                fileRef.current!.click();
                return;
            }
            const dirHandle = await window.showDirectoryPicker({ startIn: 'documents' });
            const response = await verifyPath(dirHandle.name);
            if (!response?.ok) {
                setModal({
                    ...modal,
                    active: true,
                    loadingBar: false,
                    title: 'Path Not Found',
                    description: response?.realPath || '',
                    buttons: [{
                        title: 'Close',
                    }]
                });
                return;
            }
            if (name === 'results' && !response?.empty) {
                deleteDataFolder(response?.realPath || '');
                return;
            }
            handleFiles(name, response?.realPath || '', index!);
            return;
        } catch (error) {

        }
    }
    return (
        <DivGrouped sx={styles}>
            <InputFile error={error} sx={{ width: '100%' }} data-testid={`test-id-input-${name}`} type='text' placeholder='Load File' value={route} />
            <Button color='primary' variant='contained' disabled={buttonDisabled} data-testid={`test-id-input-button-${name}`} onClick={handleClick}>Browse</Button>
            <input name={name} accept={accept} ref={fileRef} style={{ display: 'none' }} data-testid={`test-id-input-hidden-${name}`} type='file' onChange={handleChange} />
        </DivGrouped>
    )
}

export default UploadFile