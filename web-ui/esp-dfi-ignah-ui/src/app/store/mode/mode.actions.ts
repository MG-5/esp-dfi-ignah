import { createAction, props } from "@ngrx/store";
import { AdditionalVehicle } from "src/app/models/additional-vehicles";
import { FreeText } from "src/app/models/free-text";
import { Mode } from "src/app/models/mode";
import { RunningText } from "src/app/models/running-text";


// Mode

export const getMode = createAction(
  '[Mode] Get mode'
);

export const getModeSuccess = createAction(
  '[Mode] Got mode',
  props<{ mode: Mode }>()
);

export const getModeError = createAction(
  '[Mode] Error getting mode'
);

export const setMode = createAction(
  '[Mode] Set mode',
  props<{ mode: Mode }>()
);

export const setModeSuccess = createAction(
  '[Mode] Set mode successfully'
);

export const setModeError = createAction(
  '[Mode] Error setting mode'
);


// Running text

export const getRunningText = createAction(
  '[Mode] Get running text'
);

export const getRunningTextSuccess = createAction(
  '[Mode] Got running text',
  props<{ runningText: RunningText }>()
);

export const getRunningTextError = createAction(
  '[Mode] Error getting running text'
);

export const setRunningText = createAction(
  '[Mode] Set running text',
  props<{ runningText: RunningText }>()
);

export const setRunningTextSuccess = createAction(
  '[Mode] Set running text successfully'
);

export const setRunningTextError = createAction(
  '[Mode] Error setting running text'
);


// Free text

export const getFreeText = createAction(
  '[Mode] Get free text'
);

export const getFreeTextSuccess = createAction(
  '[Mode] Got free text',
  props<{ freeText: FreeText }>()
);

export const getFreeTextError = createAction(
  '[Mode] Error getting free text'
);

export const setFreeText = createAction(
  '[Mode] Set free text',
  props<{ freeText: FreeText }>()
);

export const setFreeTextSuccess = createAction(
  '[Mode] Set free text successfully'
);

export const setFreeTextError = createAction(
  '[Mode] Error setting free text'
);


// Additional vehicles

export const addVehicle = createAction(
  '[Vehicle] Add',
  props<{ vehicle: AdditionalVehicle }>()
);

export const removeVehicle = createAction(
  '[Vehicle] Remove',
  props<{ vehicleIndex: number }>()
);

export const updateVehicle = createAction(
  '[Vehicle] Update',
  props<{ vehicle: AdditionalVehicle, vehicleIndex: number }>()
);

export const fetchVehicles = createAction(
  '[Vehicle] Fetch'
);

export const fetchVehiclesSuccess = createAction(
  '[Vehicle] Fetch succeeded',
  props<{ vehicles: AdditionalVehicle[] }>()
);

export const fetchVehiclesError = createAction(
  '[Vehicle] Fetch failed'
);

export const pushVehicles = createAction(
  '[Vehicle] Push'
);

export const pushVehiclesSuccess = createAction(
  '[Vehicle] Push succeded'
);

export const pushVehiclesError = createAction(
  '[Vehicle] Push failed'
);