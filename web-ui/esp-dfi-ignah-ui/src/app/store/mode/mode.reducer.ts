import { createReducer, on } from "@ngrx/store";
import { FreeText } from "src/app/models/free-text";
import { Mode } from "src/app/models/mode";
import { RunningText } from "src/app/models/running-text";
import { getFreeTextSuccess, getModeSuccess, getRunningTextSuccess } from "./mode.actions";

export interface ModeState {
  mode: Mode;
  runningText: RunningText;
  freeText: FreeText;
}

const initialState: ModeState = {
  mode: Mode.Normal,
  runningText: {
    text: '',
    speed: 50
  },
  freeText: {
    lines: []
  }
};

export const modeReducer = createReducer(
  initialState,
  on(getModeSuccess, (state, {mode}) => ({
    ...state,
    mode
  })),
  on(getRunningTextSuccess, (state, {runningText}) => ({
    ...state,
    runningText
  })),
  on(getFreeTextSuccess, (state, {freeText}) => ({
    ...state,
    freeText
  }))
);