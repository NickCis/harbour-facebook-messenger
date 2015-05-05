(function(){
    var console_log = console.log;
    console.log = function(text){
        if(navigator && navigator.qt && navigator.qt.postMessage)
            navigator.qt.postMessage(JSON.stringify({
                type: "log",
                msg: text
            }));
        else
            console_log(text);
    };
})();
