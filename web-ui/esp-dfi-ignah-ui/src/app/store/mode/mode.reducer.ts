import { state } from "@angular/animations";
import { createReducer, on } from "@ngrx/store";
import { Mode } from "src/app/models/mode";
import { getModeSuccess } from "./mode.actions";

export interface ModeState {
  mode: Mode;
}

const initialState: ModeState = {
  mode: Mode.Normal
};

export const modeReducer = createReducer(
  initialState,
  on(getModeSuccess, (state, {mode}) => ({
    ...state,
    mode
  }))
);