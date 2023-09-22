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

import { useState } from "react"
import { ISimulationId } from "../components/interfaces/SetupConfigs/setupConfigs";

export const useForm = <Type>(initialState: Type) => {
  const [values, setValues] = useState(initialState);

  // const reset = () => {
  //   setValues(initialState)
  // }

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setValues({ ...values, [name]: value });
  }

  const handleChange = (newValues: Partial<Type>) => {
    setValues(prev => ({ ...prev, ...newValues }));
  }

  const handleChangeByName = (name: string, value: any, simulations: ISimulationId[] | null = null) => {
    if (simulations) {
      setValues({
        ...values,
        simulations: simulations,
        [name]: value
      });
      return;
    }
    setValues({ ...values, [name]: value });
  }

  return { values, handleChangeByName, handleInputChange, handleChange };
}
