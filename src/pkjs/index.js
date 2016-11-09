function locationSuccess(pos)
{
  //Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' + 
    pos.coords.latitude + '&lon=' + pos.coords.longitude + 
    '&appid=039b16db4b14d44de203180d75fd9fe6';
  
  //Send request to OpenWeatherMap
  xhrRequest(url, 'GET',
    function(responseText)
    {
      //responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      //Temperature in Kelvin requires adjustment
      var temperature = Math.round((json.main.temp * 1.8) - 459.67);
      console.log('Temperature is ' + temperature);
      //Conditions
      var conditions = json.weatehr[0].main;
      console.log('Conditions are ' + conditions);
    }
  );
}

function locationError(err)
{
  console.log('Error requesting location!');
}

function getWeather()
{
  navigator.geolocation.getCurrentPosition(
  locationSuccess,
  locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

//Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e)
  {
    console.log('PebbleKit JS ready!');
    //Get the initial weather
    getWeather();
  }
);

//Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e)
  {
    console.log('AppMessage received!');
    getWeather();
  }
);

var xhrRequest = function (url, type, callback)
{
  var xhr = newXMLHttpRequest();
  xhr.onload = function ()
  {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

var dictionary = 
{
  'TEMPERATURE': temperature,
  'CONDITIONS': conditions
};

//Send to Pebble
Pebble.sendAppMessage(dictionary,
  function(e)
  {
    console.log('Weather info sent to Pebble successfully!');
  },
  function(e)
  {
    console.log('Error sending weather info to Pebble!');
  }
);