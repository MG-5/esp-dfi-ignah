import { Component, OnInit } from '@angular/core';
import { FormBuilder, Validators, FormsModule, ReactiveFormsModule } from '@angular/forms';
import { Store } from '@ngrx/store';
import { DfiStationSettings } from 'src/app/models/dfi-station-settings';
import { getLightSensor, selectLightSensorSettings, setLightSensor } from 'src/app/store/mode';
import { MatSliderModule } from '@angular/material/slider';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
// import { getDfiStationSettings } from 'src/app/store/actions/system.actions';
// import { setDfiStationSettings } from 'src/app/store/actions/system.actions';

@Component({
    selector: 'app-system-settings',
    templateUrl: './settings.component.html',
    styleUrls: ['./settings.component.scss'],
    standalone: true,
    imports: [FormsModule, ReactiveFormsModule, MatFormFieldModule, MatInputModule, MatSliderModule]
})
export class SettingsComponent implements OnInit {

  protected readonly lightSensorMin = 50;
  protected readonly lightSensorMax = 1023;

  protected readonly lightSensorGainMin = 0;
  protected readonly lightSensorGainMax = 5;

  protected settingsForm = this.fb.group({
    station: this.fb.group({
      name: '',
      number: '',
    }),
    lightSensor: this.fb.group({
      min: [this.lightSensorMin, [Validators.min(this.lightSensorMin), Validators.max(this.lightSensorMax)]],
      max: [this.lightSensorMax, [Validators.min(this.lightSensorMin), Validators.max(this.lightSensorMax)]],
      gain: [0, [Validators.min(this.lightSensorGainMin), Validators.max(this.lightSensorGainMax)]]
    })
  });

  constructor(private store: Store, private fb: FormBuilder) { }

  ngOnInit(): void {
    //this.store.dispatch(getDfiStationSettings());
    this.store.dispatch(getLightSensor());

    this.store.select(selectLightSensorSettings).subscribe(settings => {
      this.settingsForm.patchValue({
        lightSensor: {
          min: settings.pwmMinimum,
          max: settings.pwmMaximum,
          gain: settings.pwmGain
        }
      });
    });
  }

  saveStationSettings(dfiStationSettings: DfiStationSettings): void {
    // this.store.dispatch(setDfiStationSettings({ dfiStationSettings }));
    // this.store.dispatch(setLightSensor());
  }
}
