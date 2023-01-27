import { Injectable } from "@angular/core";
import { Actions, createEffect, ofType } from "@ngrx/effects";
import { catchError, map, mergeMap, of } from "rxjs";
import { ModeService } from "src/app/services/api/mode.service";
import { getModeError, getMode, getModeSuccess, setMode, setModeSuccess, setModeError } from "./mode.actions";

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

  constructor(private actions$: Actions, private modeService: ModeService) { }

}