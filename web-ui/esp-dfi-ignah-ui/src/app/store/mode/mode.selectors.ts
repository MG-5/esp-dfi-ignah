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

export const selectFreeText = createSelector(modeFeature,
  (state: ModeState) => state.freeText
);

export const selectAdditionalVehicles = createSelector(modeFeature,
  (state: ModeState) => state.additionalVehicles
);

export const selectLightSensorSettings = createSelector(modeFeature, (state: ModeState) => state.lightSensorSettings);

export const selectDfiStationSettings = createSelector(modeFeature, (state: ModeState) => state.stationSettings);