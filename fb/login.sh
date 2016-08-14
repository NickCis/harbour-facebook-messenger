#! /usr/bin/bash

curl "https://www.messenger.com/login" > index.html

request_id=$(sed -n 's/.*"initialRequestID":"\([^"]*\)".*/\1/p' index.html)
identifier=$(sed -n 's/.*"identifier":"\([^"]*\)".*/\1/p' index.html)
js_datr=$(sed -n 's/.*"_js_datr","\([^"]*\)".*/\1/p' index.html)
lsd=$(sed -n 's/.*name="lsd" value="\([^"]*\)".*/\1/p' index.html)

echo "request_id: $request_id"
echo "identifier: $identifier"
echo "js_datr: $js_datr"
echo "lsd: $lsd"

read -p "Continue?"

rm -f cookies

# curl -v "https://www.messenger.com/login/fb_iframe_target/?userid=0&initial_request_id=$request_id"
#echo ""
#read -p "Continue?"

#curl -v -H "refer: https://www.messenger.com/login/" "https://www.facebook.com/login/messenger_dot_com_iframe/?redirect_uri=https%3A%2F%2Fwww.messenger.com%2Flogin%2Ffb_iframe_target%2F%3Finitial_request_id%3D$request_id&identifier=$identifier&initial_request_id=$request_id"
#echo ""
#read -p "Continue?"

# Esto se usa para poner la cookie en el cookie jar, es totalmente opcional! (el cookie inicialmente viene de js, y la web lo tiene que poner como HTTPONLY)
# curl -v -c cookies -H "refer: https://www.messenger.com/login/" -H "cookie: _js_datr=$js_datr" "https://www.messenger.com/login/fb_iframe_target/?userid=0&initial_request_id=$request_id"
#echo ""
#read -p "Continue?"

read -s -p "Password: " password
echo ""

curl -v -c cookies --data "lsd=$lsd&initial_request_id=$request_id&email=ncis20@gmail.com&pass=$password&default_persistent=0" -H "cookie: datr=$js_datr" "https://www.messenger.com/login/password/"
echo "#HttpOnly_.messenger.com	TRUE	/	FALSE	1497074854	datr	$js_datr" >> cookies

#curl -v -c cookies -b cookies --data "lsd=$lsd&initial_request_id=$request_id&email=ncis20@gmail.com&pass=$password&default_persistent=0" "https://www.messenger.com/login/password/"

echo ""
read -p "Continue?"

curl -v -b cookies "https://www.messenger.com/"
echo ""
