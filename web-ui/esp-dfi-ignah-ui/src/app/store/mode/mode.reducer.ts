import { createReducer, on } from "@ngrx/store";
import { FreeText } from "src/app/models/free-text";
import { Mode } from "src/app/models/mode";
import { RunningText } from "src/app/models/running-text";
import { addVehicle, fetchVehiclesSuccess, getFreeTextSuccess, getModeSuccess, getRunningTextSuccess, removeVehicle, updateVehicle } from "./mode.actions";
import { AdditionalVehicle } from "src/app/models/additional-vehicles";

export interface ModeState {
  mode: Mode;
  runningText: RunningText;
  freeText: FreeText;
  additionalVehicles: AdditionalVehicle[];
}

const initialState: ModeState = {
  mode: Mode.Normal,
  runningText: {
    text: '',
    speed: 50
  },
  freeText: {
    lines: []
  },
  additionalVehicles: []
};

export const modeReducer = createReducer(
  initialState,
  on(getModeSuccess, (state, { mode }) => ({
    ...state,
    mode
  })),
  on(getRunningTextSuccess, (state, { runningText }) => ({
    ...state,
    runningText
  })),
  on(getFreeTextSuccess, (state, { freeText }) => ({
    ...state,
    freeText
  })),
  on(addVehicle, (state, { vehicle }) => ({
    ...state,
    additionalVehicles: state.additionalVehicles.concat(vehicle)
  })),
  on(removeVehicle, (state, { vehicleIndex }) => {
    let newVehicles = [...state.additionalVehicles];
    newVehicles.splice(vehicleIndex, 1);

    return {
      ...state,
      additionalVehicles: newVehicles
    };
  }),
  on(updateVehicle, (state, { vehicleIndex, vehicle }) => {
    let newVehicles = [...state.additionalVehicles];
    newVehicles[vehicleIndex] = vehicle;

    return {
      ...state,
      additionalVehicles: newVehicles
    };
  }),
  on(fetchVehiclesSuccess, (state, { vehicles }) => ({
    ...state,
    additionalVehicles: vehicles
  }))
);