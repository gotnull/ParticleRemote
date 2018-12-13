// See https://github.com/dialogflow/dialogflow-fulfillment-nodejs
// for Dialogflow fulfillment library docs, samples, and to report issues
'use strict';

const functions = require('firebase-functions');
const {
  WebhookClient
} = require('dialogflow-fulfillment');
const {
  Card,
  Suggestion
} = require('dialogflow-fulfillment');
const Particle = require('particle-api-js');

process.env.DEBUG = 'dialogflow:debug'; // enables lib debugging statements

exports.dialogflowFirebaseFulfillment = functions.https.onRequest((request, response) => {
  const agent = new WebhookClient({
    request,
    response
  });

  console.log('Dialogflow Request headers: ' + JSON.stringify(request.headers));
  console.log('Dialogflow Request body: ' + JSON.stringify(request.body));

  const particle = new Particle();
  const access_token = 'eb861b8c0a18258f374f65d4735c2d92cef163a6';

  function remoteControl(agent) {
    let params = agent.parameters;
    let [deviceType, command] = [params['device-type'], params['command']];
    let deviceTypeSentence = `Turning the ${deviceType} ${command}.`;

    return particle.callFunction({
      deviceId: '33002c001047343339383037',
      name: 'command',
      argument: `${deviceType},${command}`,
      auth: access_token
    }).then((data) => {
        switch (deviceType) {
          case 'television':
          case 'tv':
            deviceTypeSentence = `Turning the ${deviceType} ${command}.`;
            break;
          case 'channel':
          case 'input':
          case 'source':
            deviceTypeSentence = `Changing the ${deviceType} to ${command}.`;
            break;
        }
        agent.add(deviceTypeSentence);
        console.log('Function called succesfully:', data);
      },
      (err) => {
        agent.add(`Sorry, I could not execute your request. Please try again.`);
        console.log('An error occurred:', err);
      });
  }

  function welcome(agent) {
    agent.add(`Welcome to my agent!`);
  }

  function fallback(agent) {
    agent.add(`I didn't understand`);
    agent.add(`I'm sorry, can you try again?`);
  }

  let intentMap = new Map();
  intentMap.set('actions.intent.REMOTE_CONTROL', remoteControl);
  intentMap.set('Default Welcome Intent', welcome);
  intentMap.set('Default Fallback Intent', fallback);

  agent.handleRequest(intentMap);
});
