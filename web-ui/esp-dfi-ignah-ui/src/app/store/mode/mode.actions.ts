import { createAction, props } from "@ngrx/store";
import { Mode } from "src/app/models/mode";

export const getMode = createAction(
  '[Mode] Get mode'
);

export const gotMode = createAction(
  '[Mode] Got mode',
  props<{mode: Mode}>()
);

export const errorGettingMode = createAction(
  '[Mode] Error getting mode'
);