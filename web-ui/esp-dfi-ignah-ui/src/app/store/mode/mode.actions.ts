import { createAction, props } from "@ngrx/store";
import { Mode } from "src/app/models/mode";

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