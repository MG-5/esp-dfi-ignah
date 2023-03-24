import { Component, OnDestroy, OnInit } from '@angular/core';
import { FormBuilder } from '@angular/forms';
import { Store } from '@ngrx/store';
import { Subscription } from 'rxjs';
import { getFreeText, selectFreeText, setFreeText } from 'src/app/store/mode';

@Component({
  selector: 'app-free-text',
  templateUrl: './free-text.component.html',
  styleUrls: ['./free-text.component.scss']
})
export class FreeTextComponent implements OnInit, OnDestroy {

  freeTextFormGroup = this.formBuilder.group({
    line1: '',
    line2: '',
    line3: '',
    line4: '',
    line5: ''
  });

  freeTextSubscription$: Subscription = Subscription.EMPTY;

  constructor(private formBuilder: FormBuilder, private store: Store) { }

  ngOnInit(): void {
    this.store.dispatch(getFreeText());

    this.store.select(selectFreeText).subscribe(freeText => {
      const lines = {
        line1: freeText.lines[0] ?? '',
        line2: freeText.lines[1] ?? '',
        line3: freeText.lines[2] ?? '',
        line4: freeText.lines[3] ?? '',
        line5: freeText.lines[4] ?? ''
      }

      this.freeTextFormGroup.patchValue(lines);
    });
  }

  ngOnDestroy(): void {
    this.freeTextSubscription$.unsubscribe();
  }

  saveFreeText(): void {
    const lines = [
      this.freeTextFormGroup.value.line1 ?? '',
      this.freeTextFormGroup.value.line2 ?? '',
      this.freeTextFormGroup.value.line3 ?? '',
      this.freeTextFormGroup.value.line4 ?? '',
      this.freeTextFormGroup.value.line5 ?? ''
    ];

    this.store.dispatch(setFreeText({
      freeText: {
        lines
      }
    }));
  }
}
