import { Injectable } from "@angular/core";
import { Actions, createEffect, ofType } from "@ngrx/effects";
import { catchError, map, mergeMap, of } from "rxjs";
import { ModeService } from "src/app/services/api/mode.service";
import { TextService } from "src/app/services/api/text.service";
import { getModeError, getMode, getModeSuccess, setMode, setModeSuccess, setModeError, getRunningText, getRunningTextSuccess, getRunningTextError, setRunningText, setRunningTextSuccess, setRunningTextError, getFreeText, setFreeText, setFreeTextSuccess, setFreeTextError, getFreeTextSuccess, getFreeTextError } from "./mode.actions";

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
      map(runningText => getRunningTextSuccess({runningText})),
      catchError(() => of(getRunningTextError()))
    );
  });

  setRunningText$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(setRunningText),
      mergeMap(({runningText}) => this.textService.setRunningText(runningText)),
      map(() => setRunningTextSuccess()),
      catchError(() => of(setRunningTextError()))
    );
  });

  getFreeText$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(getFreeText),
      mergeMap(() => this.textService.getFreeText()),
      map(freeText => getFreeTextSuccess({freeText})),
      catchError(() => of(getFreeTextError()))
    );
  });

  setFreeText$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(setFreeText),
      mergeMap(({freeText}) => this.textService.setFreeText(freeText)),
      map(() => setFreeTextSuccess()),
      catchError(() => of(setFreeTextError()))
    );
  });

  constructor(private actions$: Actions, private modeService: ModeService, private textService: TextService) { }

}