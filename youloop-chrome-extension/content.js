console.log("YOULOOP extension started")

if (window == top) {
    window.addEventListener('keyup', setLoop, false); //add the keyboard handler
}


var STATE_IDLE = 0;
var STATE_SET_START = 1;
var STATE_SET_END = 2;
var STATE_RESET = 3;

var state = STATE_IDLE;
var start = -1;
var end = -1;

var refresh = 500; // ms

var player = undefined;

function executeOnPageSpace(code){

    // create a script tag
    var script = document.createElement('script')
    script.id = 'tmpScript'
    // place the code inside the script. later replace it with execution result.
    script.textContent = 
    'document.getElementById("tmpScript").textContent = JSON.stringify(' + code + ')'
    // attach the script to page
    document.documentElement.appendChild(script)
    // collect execution results
    let result = document.getElementById("tmpScript").textContent
    // remove script from page
    script.remove()
    return JSON.parse(result)
  
  }

const getTimeCode = `(function getTime(){ return document.getElementById("movie_player").getCurrentTime(); })()`;

function setLoop(e){

    if (e.keyCode == 89){ // Y (e.altKey && ALT + Y)
        state += 1;

        if (state == STATE_SET_START) {
            start = executeOnPageSpace(getTimeCode);
            console.log("Set start of the loop at " + start);            
        }
        else if (state == STATE_SET_END) {
            end = executeOnPageSpace(getTimeCode);
            // loop video
            executeOnPageSpace(`(function doLoop(){ 
                function checkTime() {
                    if (document.getElementById("movie_player").getCurrentTime() >= ${end}) {
                        document.getElementById("movie_player").seekTo(${start});
                    }
                }
                timeupdater = setInterval(checkTime, ${refresh});
                return 1; 
            })()`);
            console.log("Set end of the loop at " + end);
        }
        else if (state === STATE_RESET) {            
            executeOnPageSpace(`(function clearLoop(){ clearInterval(timeupdater); return 1; })()`);
            console.log("Stopping loop");
            start = -1;
            end = -1;
            state = STATE_IDLE;
        }

	}
}
