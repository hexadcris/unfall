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

import { useTheme } from '@mui/material';
import { BASE_DIVISOR, DragComponent, ExtendedComponent, ISystemEditor } from '../components/interfaces/SystemEditor/systemEditor.d';
import { ComponentUI as Component } from 'opGUI-client-frontend';
import { useGlobalContext } from '../context/ContextProvider';
export const useCardTarget = (availableElements: Component[], handleChange: (values: Partial<ISystemEditor>) => void) => {
  const theme = useTheme();
  const { setShowDiscardModal } = useGlobalContext();
  const allowDrop = (ev: React.DragEvent<HTMLDivElement>) => {
    ev.preventDefault();
  };

  const dragStartItem = (event: React.DragEvent<HTMLDivElement>, component: Component | ExtendedComponent | DragComponent) => {
    const target = event.target as HTMLDivElement;
    const boundingRect = target.getBoundingClientRect();
    const item: DragComponent = {
      ...component,
      offsetY: event.clientY - boundingRect.top,
      offsetX: event.clientX - boundingRect.left,
      position: {
        x: (target.clientWidth / BASE_DIVISOR),
        y: (target.clientHeight / BASE_DIVISOR)
      },
      height: target.clientHeight,
      width: target.clientWidth,
      dropped: (component as DragComponent).dropped || false,
      color: (component as DragComponent).color || theme.palette.primary.main,
    };
    event.dataTransfer.clearData('text');
    event.dataTransfer.setData('text', JSON.stringify(item));
  }

  const dropElement = (event: React.DragEvent<HTMLDivElement>) => {
    event.preventDefault();
    const target = event.target as HTMLDivElement;
    if (target.id !== 'card-target') {
      return;
    }
    const boundingRect = target.getBoundingClientRect();
    const props = JSON.parse(event.dataTransfer.getData('text'));
    const clientX = event.clientX || 1080;
    const clientY = event.clientY || 304;
    const x = clientX - boundingRect.left + target.scrollLeft - props.offsetX;
    let y = clientY - boundingRect.top + target.scrollTop - props.offsetY;
    if (y < 0) y = 5;
    if (props.dropped !== true) {
      props.dropped = true;
      props.position = {
        x,
        y
      }
      handleChange({ availableElements: [...availableElements, { ...props }] });
      setShowDiscardModal(true);
      return;
    }
    const newAvailableElements = [...availableElements];
    props.position = {
      x,
      y
    }
    newAvailableElements[props.index] = props;
    handleChange({ availableElements: newAvailableElements });
    setShowDiscardModal(true);
  };

  return {
    availableElements,
    dropElement,
    allowDrop,
    dragStartItem
  };
};
