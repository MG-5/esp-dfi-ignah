import { Injectable } from "@angular/core";
import { Actions, createEffect, ofType } from "@ngrx/effects";
import { catchError, combineLatest, map, mergeMap, of, withLatestFrom } from "rxjs";
import { ModeService } from "@app/services/api/mode.service";
import { TextService } from "@app/services/api/text.service";
import { getModeError, getMode, getModeSuccess, setMode, setModeSuccess, setModeError, getRunningText, getRunningTextSuccess, getRunningTextError, setRunningText, setRunningTextSuccess, setRunningTextError, getFreeText, setFreeText, setFreeTextSuccess, setFreeTextError, getFreeTextSuccess, getFreeTextError, fetchVehicles, fetchVehiclesSuccess, fetchVehiclesError, pushVehicles, pushVehiclesSuccess, pushVehiclesError, setLightSensor, setLightSensorSuccess, setLightSensorError, getLightSensor, getLightSensorSuccess, getLightSensorError, getDfiStationSettings, setDfiStationSettings, getDfiStationSettingsSuccess, setDfiStationSettingsSuccess, setDfiStationSettingsError, getDfiStationSettingsError, fetchDestinationBlocklist, fetchDestinationBlocklistSuccess, fetchDestinationBlocklistError, pushDestinationBlocklist, pushDestinationBlocklistSuccess, pushDestinationBlocklistError } from "./mode.actions";
import { VehicleService } from "@app/services/api/vehicle.service";
import { Store } from "@ngrx/store";
import { selectAdditionalVehicles, selectDestinationBlocklist } from "./mode.selectors";
import { SystemService } from "@app/services/api/system.service";
import { DestinationBlocklistService } from "@app/services/api/destination";

@Injectable()
export class ModeEffects {

  getMode$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(getMode),
      mergeMap(() => this.modeService.getMode()),
      map(mode => getModeSuccess({ mode })),
      catchError(() => of(getModeError()))
    )
  });

  setMode$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(setMode),
      mergeMap(({ mode }) => this.modeService.setMode(mode)),
      map(() => setModeSuccess()),
      catchError(() => of(setModeError()))
    )
  });

  getRunningText$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(getRunningText),
      mergeMap(() => this.textService.getRunningText()),
      map(runningText => getRunningTextSuccess({ runningText })),
      catchError(() => of(getRunningTextError()))
    );
  });

  setRunningText$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(setRunningText),
      mergeMap(({ runningText }) => this.textService.setRunningText(runningText)),
      map(() => setRunningTextSuccess()),
      catchError(() => of(setRunningTextError()))
    );
  });

  getFreeText$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(getFreeText),
      mergeMap(() => this.textService.getFreeText()),
      map(freeText => getFreeTextSuccess({ freeText })),
      catchError(() => of(getFreeTextError()))
    );
  });

  setFreeText$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(setFreeText),
      mergeMap(({ freeText }) => this.textService.setFreeText(freeText)),
      map(() => setFreeTextSuccess()),
      catchError(() => of(setFreeTextError()))
    );
  });

  fetchVehicles$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(fetchVehicles),
      mergeMap(() => this.vehicleService.getAdditionalVehicles()),
      map(data => fetchVehiclesSuccess({ vehicles: data.vehicles })),
      catchError(() => of(fetchVehiclesError()))
    );
  });

  pushVehicles$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(pushVehicles),
      withLatestFrom(this.store.select(selectAdditionalVehicles)),
      mergeMap(([_, additionalVehicles]) => this.vehicleService.setAdditionalVehicles({ vehicles: additionalVehicles })),
      map(() => pushVehiclesSuccess()),
      catchError(() => of(pushVehiclesError()))
    );
  });

  getLightSensor$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(getLightSensor),
      mergeMap(() => this.systemService.getLightSensor()),
      map((settings) => getLightSensorSuccess({ settings })),
      catchError(() => of(getLightSensorError()))
    );
  });

  setLightSensor$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(setLightSensor),
      mergeMap(({ settings }) => this.systemService.setLightSensor(settings)),
      map(() => setLightSensorSuccess()),
      catchError(() => of(setLightSensorError()))
    );
  });

  getDfiStationSettings$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(getDfiStationSettings),
      mergeMap(() => this.systemService.getDfiStationSettings()),
      map((settings) => getDfiStationSettingsSuccess({ settings })),
      catchError(() => of(getDfiStationSettingsError()))
    );
  });

  setDfiStationSettings$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(setDfiStationSettings),
      mergeMap(({ settings }) => this.systemService.setDfiStationSettings(settings)),
      map(() => setDfiStationSettingsSuccess()),
      catchError(() => of(setDfiStationSettingsError()))
    );
  });

  fetchDestinationBlocklist$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(fetchDestinationBlocklist),
      mergeMap(() => this.destinationBlocklistService.getDestinationBlocklist()),
      map(data => fetchDestinationBlocklistSuccess({ destinations: data.blocklist })),
      catchError(() => of(fetchDestinationBlocklistError()))
    );
  });

  pushDestinationBlocklist$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(pushDestinationBlocklist),
      withLatestFrom(this.store.select(selectDestinationBlocklist)),
      mergeMap(([_, destinations]) => this.destinationBlocklistService.setDestinationBlocklist({ blocklist: destinations.blocklist })),
      map(() => pushDestinationBlocklistSuccess()),
      catchError(() => of(pushDestinationBlocklistError()))
    );
  });



  constructor(
    private actions$: Actions,
    private store: Store,
    private modeService: ModeService,
    private textService: TextService,
    private vehicleService: VehicleService,
    private systemService: SystemService,
    private destinationBlocklistService: DestinationBlocklistService) { }

}