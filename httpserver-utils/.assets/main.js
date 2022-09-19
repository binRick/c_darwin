$(document).ready(function() {
    console.log("main.js");


const socket = new WebSocket('ws://localhost:49226');

// Connection opened
socket.addEventListener('open', (event) => {
  setInterval(function(){
    socket.send('Hello Server!');
  }, 5000);
});

// Listen for messages
socket.addEventListener('message', (event) => {
    console.log('Message from server ', event.data);
});


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
        trigger: 'left',
        zIndex: 100,
        delay: 100,
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
        // there's more, have a look at the demos and docs...
    });

    $.getJSON("/config", function(cfg) {
        window.cfg = cfg;
        console.log("Config>", window.cfg);
    });
});
