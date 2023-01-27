import { Injectable } from "@angular/core";
import { Actions, createEffect, ofType } from "@ngrx/effects";
import { catchError, map, mergeMap, of } from "rxjs";
import { ModeService } from "src/app/services/api/mode.service";
import { errorGettingMode, getMode, gotMode } from "./mode.actions";

@Injectable()
export class ModeEffects {

  getMode$ = createEffect(() => {
    return this.actions$.pipe(
      ofType(getMode),
      mergeMap(() => this.modeService.getMode()),
      map(mode => gotMode({ mode })),
      catchError(() => of(errorGettingMode()))
    )
  });

  constructor(private actions$: Actions, private modeService: ModeService) { }

}