import { state } from "@angular/animations";
import { createReducer, on } from "@ngrx/store";
import { Mode } from "src/app/models/mode";
import { RunningText } from "src/app/models/running-text";
import { getModeSuccess, getRunningTextSuccess } from "./mode.actions";

export interface ModeState {
  mode: Mode;
  runningText: RunningText;
}

const initialState: ModeState = {
  mode: Mode.Normal,
  runningText: {
    text: '',
    speed: 50
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
  }))
);