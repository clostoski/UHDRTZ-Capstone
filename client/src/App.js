import React, { Component } from 'react';
import './App.css';
import axios from 'axios';
import data from './output.json';

const api = axios.create({
  baseURL: 'http://localhost:3000'
})

const move_amount = 10;

class App extends React.Component {


  constructor(props) {
    super(props);

    this.state = {
      xPos: 0,
      yPos: 0,
      zoom: 0,
      mask: 0,
      crosshair: 0,
      calibrate: 0
    }

    this.state = data;
  }

  componentDidUpdate(prevState) {
    if (this.state !== prevState) {
      this.submit();

      if (this.state.calibrate === 1) {
        this.setState({ calibrate: 0 });
      }
    }
  }

  moveUp = e => {
    e.preventDefault();

    this.setState({ yPos: this.state.yPos + move_amount });
  }

  moveDown = e => {
    e.preventDefault();

    this.setState({ yPos: this.state.yPos - move_amount });
  }

  moveLeft = e => {
    e.preventDefault();

    this.setState({ xPos: this.state.xPos - move_amount });
  }

  moveRight = e => {
    e.preventDefault();

    this.setState({ xPos: this.state.xPos + move_amount });
  }

  adjustZoom = e => {
    e.preventDefault();

    var zoomVal = parseInt(document.getElementById("zoom-slider").value);
    this.setState({ zoom: zoomVal });
  }

  calibrate = e => {
    e.preventDefault();

    this.setState({ calibrate: 1 });
  }

  reset = e => {
    e.preventDefault();

    this.setState({ xPos: 0, yPos: 0, zoom: 0 });
  }

  maskToggle = e => {
    e.preventDefault();

    var maskVal = document.getElementById("mask-toggle").value;
    this.setState({ mask: maskVal });
  }

  crosshairToggle = e => {
    e.preventDefault();

    var crosshairVal = document.getElementById("crosshair-toggle").value;
    this.setState({ crosshair: crosshairVal });
  }

  submit() {
    console.log(this.state);
    api
      .post('/', this.state)
      .then(() => console.log('Posted to api'))
      .catch(err => {
        console.error(err);
      });
  }


  render() {
    return (
      <div className="App">
        <header class="App-header">
          <div class="col-12">
            HDRTZ Configuration Menu
          </div>
        </header>


        <body class="App-body">

          <div class="row">
            <div class="col-3">
            </div>
            <div class="col-6">

              <div class="row">
                <div class="col-12">
                  Move Camera:
                </div>
              </div>
              <div class="row">
                <div class="col-3">
                  <button type="button" class="btn btn-color col-10" onClick={this.moveUp}>Up</button>
                </div>
                <div class="col-3">
                  <button type="button" class="btn btn-color col-10" onClick={this.moveDown}>Down</button>
                </div>
                <div class="col-3">
                  <button type="button" class="btn btn-color col-10" onClick={this.moveLeft}>Left</button>
                </div>
                <div class="col-3">
                  <button type="button" class="btn btn-color col-10" onClick={this.moveRight}>Right</button>
                </div>
              </div>
              <div class="row">
                &nbsp;
              </div>
              <div class="row">
                <div class="col-12">
                  Zoom:
                </div>
              </div>

              <div class="row">
                <div class="col-1">1x</div>
                <div class="col-10">
                  <input id="zoom-slider" type="range" class="form-range" defaultValue={this.state.zoom} onChange={this.adjustZoom} min="0" max="100"></input>
                </div>
                <div class="col-1">4x</div>
              </div>
              <div class="row">
                &nbsp;
              </div>
              <div class="row">
                <div class="col-3">
                  Mask:
                </div>
                <div class="col-3">
                  <label class="switch switch-flat switch-format">
                    <input id="mask-toggle" class="switch-input" type="checkbox" onChange={this.maskToggle} />
                    <span class="switch-label" data-on="On" data-off="Off"></span>
                    <span class="switch-handle"></span>
                  </label>
                </div>
                <div class="col-3">
                  Crosshair:
                </div>
                <div class="col-3">
                  <label class="switch switch-flat switch-format">
                    <input id="crosshair-toggle" class="switch-input" type="checkbox" onChange={this.crosshairToggle} />
                    <span class="switch-label" data-on="On" data-off="Off"></span>
                    <span class="switch-handle"></span>
                  </label>
                </div>
                <div class="row">
                  &nbsp;
                </div>
                <div class="row">
                  <div class="col-6">
                    <button type="button" class="btn btn-color col-10" onClick={this.calibrate}>Calibrate Crank When Idle</button>
                  </div>
                  <div class="col-6">
                    <button type="button" class="btn btn-color col-10" onClick={this.reset}>Reset Position</button>
                  </div>
                </div>
              </div>
            </div>
            <div class="col-3">
            </div>
          </div>
        </body>
      </div>
    );
  }
}

export default App;
