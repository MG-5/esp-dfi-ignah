import { createReducer, on } from "@ngrx/store";
import { Mode } from "src/app/models/mode";
import { gotMode } from "./mode.actions";

export interface ModeState {
  mode: Mode;
}

const initialState: ModeState = {
  mode: Mode.Normal
};

export const modeReducer = createReducer(
  initialState,
  on(gotMode, (state, {mode}) => state.mode = mode)
);