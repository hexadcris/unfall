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

import { createContext, useContext, useState, ReactNode, Dispatch, SetStateAction, useRef } from "react";
import { IModal, PATHS } from "../index.d";

interface GlobalContent {
    modal: IModal;
    setModal: Dispatch<SetStateAction<IModal>>;
    sideWindowRef: React.RefObject<HTMLDivElement> | null;
    scrollByPixels: (pixels: number) => void;
    selectedTab: PATHS;
    setSelectedTab: Dispatch<SetStateAction<PATHS>>;
    setProgress: Dispatch<SetStateAction<number>>;
    progress: number;
    showDiscardModal: boolean;
    setShowDiscardModal: Dispatch<SetStateAction<boolean>>;
}

interface GlobalContextProviderProps {
    children: ReactNode;
}

export const GlobalContext = createContext<GlobalContent>({
    modal: {
        title: "",
        active: false,
        description: "",
        buttons: [],
        loadingBar: false
    },
    setModal: () => { },
    scrollByPixels: () => { },
    sideWindowRef: null,
    selectedTab: PATHS.HOME,
    setSelectedTab: () => { },
    setProgress: () => { },
    progress: 0,
    showDiscardModal: false,
    setShowDiscardModal: () => { }
});

export const GlobalContextProvider: React.FC<GlobalContextProviderProps> = ({
    children
}) => {
    const [showDiscardModal, setShowDiscardModal] = useState(false);
    const [selectedTab, setSelectedTab] = useState(PATHS.HOME);
    const [progress, setProgress] = useState(0);
    const [modal, setModal] = useState<IModal>({
        title: "",
        active: false,
        description: "",
        buttons: [],
        loadingBar: false
    });
    const sideWindowRef = useRef<HTMLDivElement>(null);
    const scrollByPixels = (pixels: number) => {
        if (sideWindowRef!.current) {
            sideWindowRef?.current.scrollTo({
                top: sideWindowRef?.current.scrollTop + pixels,
                behavior: 'smooth',
            });
        }
    };
    return (
        <GlobalContext.Provider
            value={
                {
                    modal,
                    setModal,
                    sideWindowRef,
                    scrollByPixels,
                    selectedTab,
                    setSelectedTab,
                    progress,
                    setProgress,
                    showDiscardModal,
                    setShowDiscardModal
                }
            }>
            {children}
        </GlobalContext.Provider>
    );
};

export const useGlobalContext = () => useContext(GlobalContext);
