(function() {
    var isConversation = false,
        currentTouch;
    (function(){
        var meta = document.createElement('meta');
        meta.setAttribute('name', 'viewport');
        if (screen.width <= 540)
            meta.setAttribute('content', 'width=device-width/1.5, initial-scale='+(1.5)+', maximum-scale='+(1.5)+', user-scalable=no');
        else if (screen.width > 540 && screen.width <= 768)
            meta.setAttribute('content', 'width=device-width/2.0, initial-scale='+(2.0)+', maximum-scale='+(2.0)+', user-scalable=no');
        else if (screen.width > 768)
            meta.setAttribute('content', 'width=device-width/3.0, initial-scale='+(3.0)+', maximum-scale='+(3.0)+', user-scalable=no');
        else
            meta.setAttribute('content', 'width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no');
        document.head.appendChild(meta);
        console.log("inserte meta");
    })();

    var contactListQuery = "[data-reactid='.0.1.$0']",
        conversationQuery = "[data-reactid='.0.1.$1']";

    function showConversation(){
        console.log("show Conversation");
        document.querySelector(contactListQuery).classList.add("hidden");
        document.querySelector(conversationQuery).classList.remove("hidden");
        isConversation = true;
    }

    function showContactList(){
        console.log("show ContactList");
        document.querySelector(conversationQuery).classList.add("hidden");
        document.querySelector(contactListQuery).classList.remove("hidden");
        isConversation = false;
    }

    document.addEventListener('touchstart', function(ev){
        if(ev.touches.length === 1)
            currentTouch = event.touches[0];

    }, false);

    document.addEventListener('touchend', function(ev){
        currentTouch = null;
    }, false);

    function pointDistance(t1, t2){
        return [t1.clientX - t2.clientX, t1.clientY - t2.clientY];
    }

    document.addEventListener('touchmove', function(ev){
        var distance = pointDistance(event.changedTouches[0], currentTouch);
        console.log("distance: "+distance[0]+ " y: "+distance[1]);
        if(isConversation){
            if(distance[0] > 200)
                showContactList();
        }else{
            document.querySelector("[data-reactid='.0.1.$0.0.1.1.0.0']").scrollTop -= distance[1];
            if(distance[0] < -200)
                showConversation();
        }
    }, false);

    window.addEventListener('load', function(){
        console.log("cargue!");
        showContactList();

        document.querySelector("[data-reactid='.0.1.$0.0.1.1.0.0.0.0.1.0']").addEventListener('click', function(ev){
            if(! ev.target.getAttribute('aria-haspopup'))
                showConversation();
        }, false);
    }, false);
})();

