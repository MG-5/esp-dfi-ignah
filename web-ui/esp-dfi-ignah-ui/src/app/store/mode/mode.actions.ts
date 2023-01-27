import { createAction, props } from "@ngrx/store";
import { Mode } from "src/app/models/mode";
import { RunningText } from "src/app/models/running-text";


// Mode

export const getMode = createAction(
  '[Mode] Get mode'
);

export const getModeSuccess = createAction(
  '[Mode] Got mode',
  props<{mode: Mode}>()
);

export const getModeError = createAction(
  '[Mode] Error getting mode'
);

export const setMode = createAction(
  '[Mode] Set mode',
  props<{mode: Mode}>()
);

export const setModeSuccess = createAction(
  '[Mode] Set mode successfully'
);

export const setModeError = createAction(
  '[Mode] Error setting mode'
);


// Running text

export const getRunningText = createAction(
  '[RunningText] Get running text'
);

export const getRunningTextSuccess = createAction(
  '[RunningText] Got running text',
  props<{runningText: RunningText}>()
);

export const getRunningTextError = createAction(
  '[RunningText] Error getting running text'
);

export const setRunningText = createAction(
  '[RunningText] Set running text',
  props<{runningText: RunningText}>()
);

export const setRunningTextSuccess = createAction(
  '[RunningText] Set running text successfully'
);

export const setRunningTextError = createAction(
  '[RunningText] Error setting running text'
);