import { createReducer, on } from "@ngrx/store";
import { FreeText } from "@app/models/free-text";
import { Mode } from "@app/models/mode";
import { RunningText } from "@app/models/running-text";
import { addVehicle, fetchVehiclesSuccess, getFreeTextSuccess, getLightSensorSuccess, getModeSuccess, getRunningTextSuccess, getDfiStationSettingsSuccess, removeVehicle, updateVehicle, addDestination, removeDestination, updateDestination, fetchDestinationBlocklistSuccess } from "./mode.actions";
import { AdditionalVehicle } from "@app/models/additional-vehicles";
import { LightSensorSettings } from "@app/models/light-sensor";
import { DfiStationSettings } from "@app/models/dfi-station-settings";
import { DestinationBlocklist } from "@app/models/destination-blocklist";

export interface ModeState {
  mode: Mode;
  runningText: RunningText;
  freeText: FreeText;
  additionalVehicles: AdditionalVehicle[];
  lightSensorSettings: LightSensorSettings;
  stationSettings: DfiStationSettings;
  destinations: DestinationBlocklist;
}

const initialState: ModeState = {
  mode: Mode.Normal,
  runningText: {
    text: '',
    speed: 35
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
    name: '',
    number: -1,
  },
  destinations: {
    blocklist: [],
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

  // Additional Vehicles
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

  // Light Sensor
  on(getLightSensorSuccess, (state, { settings }) => ({
    ...state,
    lightSensorSettings: settings
  })),

  // DFI Station Settings
  on(getDfiStationSettingsSuccess, (state, { settings }) => ({
    ...state,
    stationSettings: settings
  })),

  // Destination Blocklist
  on(addDestination, (state, { destination }) => ({
    ...state,
    destinations: { ...state.destinations, blocklist: state.destinations.blocklist.concat(destination) }
  })),
  on(removeDestination, (state, { destinationIndex }) => {
    const newBlocklist = [...state.destinations.blocklist];
    newBlocklist.splice(destinationIndex, 1);

    return {
      ...state,
      destinations: { ...state.destinations, blocklist: newBlocklist }
    };
  }),
  on(updateDestination, (state, { destinationIndex, destination }) => {
    const newBlocklist = [...state.destinations.blocklist];
    newBlocklist[destinationIndex] = destination;

    return {
      ...state,
      destinations: { ...state.destinations, blocklist: newBlocklist }
    };
  }),
  on(fetchDestinationBlocklistSuccess, (state, { destinations }) => ({
    ...state,
    destinations: { ...state.destinations, blocklist: destinations }
  }))
);