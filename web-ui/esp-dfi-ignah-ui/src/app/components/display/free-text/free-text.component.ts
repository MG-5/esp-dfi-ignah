import { Component } from '@angular/core';
import { FormBuilder } from '@angular/forms';

@Component({
  selector: 'app-free-text',
  templateUrl: './free-text.component.html',
  styleUrls: ['./free-text.component.scss']
})
export class FreeTextComponent {

  freeTextFormGroup = this.formBuilder.group({
    line1: '',
    line2: '',
    line3: '',
    line4: '',
    line5: ''
  });

  constructor(private formBuilder: FormBuilder) {}

}
