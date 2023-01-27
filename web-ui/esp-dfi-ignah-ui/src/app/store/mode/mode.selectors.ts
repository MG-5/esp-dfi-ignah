import { createFeatureSelector, createSelector } from "@ngrx/store";
import { ModeState } from "./mode.reducer";

export const modeFeatureKey = 'mode';
export const modeFeature = createFeatureSelector<ModeState>(modeFeatureKey);

export const selectMode = createSelector(modeFeature,
  (state: ModeState) => state.mode
);

export const selectRunningText = createSelector(modeFeature,
  (state: ModeState) => state.runningText
);