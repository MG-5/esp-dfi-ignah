import { createReducer, on } from "@ngrx/store";
import { FreeText } from "src/app/models/free-text";
import { Mode } from "src/app/models/mode";
import { RunningText } from "src/app/models/running-text";
import { addVehicle, fetchVehiclesSuccess, getFreeTextSuccess, getLightSensorSuccess, getModeSuccess, getRunningTextSuccess, getDfiStationSettingsSuccess, removeVehicle, updateVehicle } from "./mode.actions";
import { AdditionalVehicle } from "src/app/models/additional-vehicles";
import { LightSensorSettings } from "src/app/models/light-sensor";
import { DfiStationSettings } from "src/app/models/dfi-station-settings";

export interface ModeState {
  mode: Mode;
  runningText: RunningText;
  freeText: FreeText;
  additionalVehicles: AdditionalVehicle[];
  lightSensorSettings: LightSensorSettings;
  stationSettings: DfiStationSettings;
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
  additionalVehicles: [],
  lightSensorSettings: {
    pwmMinimum: 50,
    pwmMaximum: 1023,
    pwmGain: 0,
  },
  stationSettings: {
    blocklist: [],
    name: '',
    number: -1,
  }
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
  })),
  on(getLightSensorSuccess, (state, { settings }) => ({
    ...state,
    lightSensorSettings: settings
  })),
  on(getDfiStationSettingsSuccess, (state, { settings }) => ({
    ...state,
    stationSettings: settings
  })),
);