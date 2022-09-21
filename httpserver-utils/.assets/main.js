var l = console.log,
  websocket_enabled = false;

var reveal_fxn = function() {
    $.getJSON("/config", function(cfg) {





        window.cfg = cfg;
        console.log("Config>", window.cfg);
        console.log(window.cfg.config.spaces);
        var section_items = [{
            plural: 'windows',
            single: 'window',
            idref: 'window_id'
        }, {
            plural: 'spaces',
            single: 'space',
            idref: 'id',
        }, {
            plural: 'displays',
            single: 'display',
            idref: 'id',
        }, ];
        $.each(section_items, function(iindex, i) {
            l(i, $("section." + i.plural));
            $("section." + i.single).remove();
            $.each(window.cfg.config[i.plural], function(index, item) {
                if (i.single != 'window' || (i.single == 'window' && item.layer == 0)) {
                  var title =  i.single + ' #' + item[i.idref];
                  var subtitle =  i.single + ' #' + item[i.idref];
                  l('Creating', i.single, index, item);
                  l(item);
                  if(i.single=='window'){
                    if(item.name.length>0)
                      title = title.concat(' ', '|', item.name,'|');
                    title = title.concat(' ', '|', item.width,'x',item.height,'|');
                    title = title.concat(' ', '|PID:', item.pid,'|');
                  }
                  var space_item = document.createElement("section");
                  var url = '/capture?type='+i.single+'&id='+item[i.idref];
                  var preview=url+'&preview=1';
                  var thumbnail=url+'&thumbnail=1';
                  var full=url;
                  var new_html = '' +
                        ' <section class="' + i.single + '">\n' +
                        '  <h3 class="r-fit-text">'+title+'</h3>'+
                        '  <img data-full="'+full+'" data-thumbnail="'+thumbnail+'" data-preview="'+preview+'" alt="' + item.uuid + '" class="capture r-stretch" src="'+thumbnail+'" style="width: 100%; margin: 0 auto 4rem auto; background: transparent;" >\n' +
                        ' </section>' +
                        '\n';
                    $(new_html).addClass(i.single);
                    l(new_html);
                    $('section.' + i.plural).append($(new_html));
                }
            });
        });

        let deck1 = new Reveal(document.querySelector('.deck1'), {
            embedded: true,
            progress: true,
            keyboardCondition: 'focused',
            plugins: [RevealHighlight, ]
        });
        deck1.on('slidechanged', (event) => {
            console.log('Deck 1 slide changed');
            $(event.currentSlide).find('img').each(function() {
              if($(this).data('full') != $(this).attr('src')){
                console.log("Reloading", $(this).attr('src'));
                $(this).removeAttr("src").attr("src", $(this).data('full'));
                l($(this), 'Reload complete:', $(this).length);
              }
            });
        });
        deck1.initialize({
          embedded: true,
            slideNumber: 'h/v',
            navigationMode: 'grid',
            preloadIframes: true,
            previewLinks: true,
            mouseWheel: true,
            fragmentInURL: true,

        }).then(() => {
            var fi = $('section').first().find('img').first();
            var url = fi.attr("src");
            l(url);

            fi.removeAttr("src").attr("src", url);
            deck1.slide(1);
        });


    });
};

$(document).ready(function() {
    console.log("main.js");
    $("#layout").width($(window).width()-5);
    $("#layout").height($(window).height()-5);
    $("#sidebar").width($("#left").width()-5);



  if(websocket_enabled){
    var socket_reconnect_interval,
        websocket_config = {
            uri: "ws://localhost:49226",
            reconnect_interval: 5000,
        },
        socket_connect = function() {
            console.log('Socket connecting!');
            window.socket = new WebSocket(websocket_config.uri);
        };

    window.socket = new WebSocket(websocket_config.uri);
    window.socket.addEventListener('close', (event) => {
        console.log('Socket closed!');
        socket_reconnect_interval = setInterval(socket_connect, websocket_config.reconnect_interval);
    });
    window.socket.addEventListener('open', (event) => {
        console.log("socket opened");

        reveal_fxn();

        clearInterval(socket_reconnect_interval);
        setInterval(function() {
            window.socket.send('Hello Server!');
        }, 60000);
    });
    window.socket.addEventListener('message', (event) => {
        console.log('Message from server ', event.data);
    });
  }else{
        reveal_fxn();
  }





    $.contextMenu.types.myType = function(item, opt, root) {
        $('<span>' + item.customName + '</span>').appendTo(this);
        this.on('contextmenu:focus', function(e) {
            // setup some awesome stuff
        }).on('contextmenu:blur', function(e) {
            // tear down whatever you did
        }).on('keydown', function(e) {
            // some funky key handling, maybe?
        });
    };



    $.contextMenu({
        selector: ".capture",
        autoHide: true,
        selectableSubMenu: true,
        trigger: 'right',
        zIndex: 100,
        delay: 10,
        command1: {
            name: "Foobar",
            type: "text",
            value: "default value"
        },
        events: {
            show: function(options) {
                this.addClass('currently-showing-menu');
                console.log('Menu show');

            },
            hide: function(options) {
                console.log('Menu hidden');
            },
            activated: function(options) {
                console.log('Menu Activated');
            }
        },
        build: function($triggerElement, e) {
            return {
                callback: function() {},
                items: {
                    menuItem: {
                        name: "My on demand menu item"
                    }
                }
            };
        },
        items: {
            "edit": {
                "name": "Edit",
                "icon": "edit"
            },
            "cut": {
                "name": "Cut",
                "icon": "cut"
            },
            "sep1": "---------",
            "quit": {
                "name": "Quit",
                "icon": "quit"
            },
            "sep2": "---------",
            "fold1": {
                "name": "Sub group",
                "items": {
                    "fold1-key1": {
                        "name": "Foo bar"
                    },
                    "fold2": {
                        "name": "Sub group 2",
                        "items": {
                            "fold2-key1": {
                                "name": "alpha"
                            },
                            "fold2-key2": {
                                "name": "bravo"
                            },
                            "fold2-key3": {
                                "name": "charlie"
                            }
                        }
                    },
                    "fold1-key3": {
                        "name": "delta"
                    }
                }
            },
            label: {
                type: "myType",
                customName: "Foo Bar"
            },
            "fold1a": {
                "name": "Other group",
                "items": {
                    "fold1a-key1": {
                        "name": "echo"
                    },
                    "fold1a-key2": {
                        "name": "foxtrot"
                    },
                    "fold1a-key3": {
                        "name": "golf"
                    }
                }
            },

            firstCommand: {
                name: "Copy",
                dataAttr: {
                    menuTitle: "My custom title"
                }
            },
            name: {
                name: "Text",
                type: 'text',
                value: "Hello World",
                events: {
                    keyup: function(e) {
                        // add some fancy key handling here?
                        window.console && console.log('key: ' + e.keyCode);
                    }
                }
            },
            sep1: "---------",
            // <input type="checkbox">
            yesno: {
                name: "Boolean",
                type: 'checkbox',
                selected: true
            },
            sep2: "---------",
            // <input type="radio">
            radio1: {
                name: "Radio1",
                type: 'radio',
                radio: 'radio',
                value: '1'
            },
            radio2: {
                name: "Radio2",
                type: 'radio',
                radio: 'radio',
                value: '2',
                selected: true
            },
            sep3: "---------",
            select: {
                name: "Select",
                type: 'select',
                options: {
                    1: 'one',
                    2: 'two',
                    3: 'three'
                },
                selected: 2
            },
            sep4: "---------",
            key: {
                name: "Something Clickable",
                callback: $.noop
            },
            separator1: "-----",
            separator2: {
                "type": "cm_separator"
            },
            foo: {
                name: "Foo",
                callback: function(key, opt) {
                    alert("Foo!");
                }
            },
            bar: {
                name: "Bar",
                callback: function(key, opt) {
                    alert("Clicked on " + key + " on element " + opt.$trigger.attr("id"));
                }
            },
        }
    });

});
