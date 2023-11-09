import { Component } from '@angular/core';
import { FormBuilder, Validators, FormsModule, ReactiveFormsModule } from '@angular/forms';
import { Store } from '@ngrx/store';
import { DfiStationSettings } from 'src/app/models/dfi-station-settings';
import { getDfiStationSettings, getLightSensor, selectDfiStationSettings, selectLightSensorSettings, setLightSensor, setDfiStationSettings } from 'src/app/store/mode';
import { MatSliderModule } from '@angular/material/slider';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatButtonModule } from '@angular/material/button';
import { takeUntilDestroyed } from '@angular/core/rxjs-interop';
import { LightSensorSettings } from 'src/app/models/light-sensor';

@Component({
    selector: 'app-system-settings',
    templateUrl: './settings.component.html',
    styleUrls: ['./settings.component.scss'],
    standalone: true,
    imports: [FormsModule, ReactiveFormsModule, MatFormFieldModule, MatInputModule, MatSliderModule, MatButtonModule]
})
export class SettingsComponent {

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

  constructor(private store: Store, private fb: FormBuilder) { 
    this.store.dispatch(getDfiStationSettings());
    this.store.dispatch(getLightSensor());

    this.store.select(selectDfiStationSettings).pipe(takeUntilDestroyed()).subscribe((settings: DfiStationSettings) => {
      this.settingsForm.patchValue({
        station: {
          name: settings.name,
          number: settings.number.toString(),
        }
      });
    });

    this.store.select(selectLightSensorSettings).subscribe((settings: LightSensorSettings) => {
      this.settingsForm.patchValue({
        lightSensor: {
          min: settings.pwmMinimum,
          max: settings.pwmMaximum,
          gain: settings.pwmGain
        }
      });
    });
   }

  protected saveStationSettings(): void {
    const stationSettings: DfiStationSettings = {
      name: this.settingsForm.value.station?.name ?? '',
      number: +(this.settingsForm.value.station?.number ?? -1),
      blocklist: [],
    };

    const lightSensorSettings: LightSensorSettings = {
      pwmMinimum: this.settingsForm.value.lightSensor?.min ?? 0,
      pwmMaximum: this.settingsForm.value.lightSensor?.max ?? 0,
      pwmGain: this.settingsForm.value.lightSensor?.gain ?? 0,
    };

    this.store.dispatch(setDfiStationSettings({ settings: stationSettings }));
    this.store.dispatch(setLightSensor({ settings: lightSensorSettings }));
  }
}
