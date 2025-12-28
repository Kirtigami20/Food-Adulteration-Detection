$(function () {
    class GaugeChart {
      constructor(element, params) {
        this._element = element;
        this._initialValue = params.initialValue;
        this._higherValue = params.higherValue;
        this._title = params.title;
        this._subtitle = params.subtitle;
      }
  
      _buildConfig() {
        let element = this._element;
  
        return {
          value: this._initialValue,
          valueIndicator: {
            color: "#fff",
          },
  
          geometry: {
            startAngle: 180,
            endAngle: 360,
          },
  
          scale: {
            startValue: 0,
            endValue: this._higherValue,
            customTicks: [0, 250, 500, 780, 1050, 1300, 1560],
            tick: {
              length: 8,
            },
  
            label: {
              font: {
                color: "#87959f",
                size: 9,
                family: '"Open Sans", sans-serif',
              },
            },
          },
  
          title: {
            verticalAlignment: "bottom",
            text: this._title,
            font: {
              family: '"Open Sans", sans-serif',
              color: "#fff",
              size: 15,
            },
  
            subtitle: {
              text: this._subtitle,
              font: {
                family: '"Open Sans", sans-serif',
                color: "#fff",
                weight: 700,
                size: 20,
              },
            },
          },
  
          onInitialized: function () {
            let currentGauge = $(element);
            let circle = currentGauge.find(".dxg-spindle-hole").clone();
            let border = currentGauge.find(".dxg-spindle-border").clone();
  
            currentGauge.find(".dxg-title text").first().attr("y", 48);
            currentGauge.find(".dxg-title text").last().attr("y", 28);
            currentGauge.find(".dxg-value-indicator").append(border, circle);
          },
        };
      }
  
      init() {
        $(this._element).dxCircularGauge(this._buildConfig());
      }
    }
  
    $(document).ready(function () {
      $("#gauge1").each(function (index, item) {
        let params = {
          initialValue: 780,
          higherValue: 1560,
          title: `Sensor Values`,
          subtitle: "AlcoholValue",
        };
  
        let gauge = new GaugeChart(item, params);
        gauge.init();
      });
      $("#gauge2").each(function (index, item) {
        let params = {
          initialValue: 780,
          higherValue: 1560,
          title: `Sensor Values`,
          subtitle: "MethaneValue",
        };
  
        let gauge = new GaugeChart(item, params);
        gauge.init();
      });
      $("#gauge3").each(function (index, item) {
        let params = {
          initialValue: 780,
          higherValue: 1560,
          title: `Sensor Values`,
          subtitle: "TurbidityValue",
        };
  
        let gauge = new GaugeChart(item, params);
        gauge.init();
      });
  
      function updateGauges() {
        $("#gauge1").each(function (index, item) {
          let gauge = $(item).dxCircularGauge("instance");
          const numberElement = document.getElementById("alcohol");
          const numberString = numberElement.innerHTML;
          const numberInt = parseInt(numberString) * 0.3809;
          console.log(numberInt); // This will output the number as an integer
          gauge.value(numberInt);
        
        });
  
        $("#gauge2").each(function (index, item) {
          let gauge = $(item).dxCircularGauge("instance");
          const numberElement = document.getElementById("methane");
          const numberString = numberElement.innerHTML;
          const numberInt = parseInt(numberString) * 0.3809;
          console.log(numberInt); // This will output the number as an integer
          gauge.value(numberInt);
        });
  
        $("#gauge3").each(function (index, item) {
          let gauge = $(item).dxCircularGauge("instance");
          const numberElement = document.getElementById("turbidity");
          const numberString = numberElement.innerHTML;
          const numberInt = parseInt(numberString) * 0.3809;
          console.log(numberInt); // This will output the number as an integer
          gauge.value(numberInt);
        });
      }
  
      // Initial call to update gauges
      updateGauges();
  
      // Update gauges every 1 second (adjust interval as needed)
      setInterval(updateGauges, 1000);
    });
  });
  