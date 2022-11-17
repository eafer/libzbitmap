/*
 * Copyright (C) 2010 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Utilities taken from WebCore/inspector/front-end/utilities.js with some additions for Safari.

Object.type = function(obj, win)
{
    if (obj === null)
        return "null";

    var type = typeof obj;
    if (type !== "object" && type !== "function")
        return type;

    win = win || window;

    if (obj instanceof win.Node)
        return "node";
    if (obj instanceof win.String)
        return "string";
    if (obj instanceof win.Array)
        return "array";
    if (obj instanceof win.Boolean)
        return "boolean";
    if (obj instanceof win.Number)
        return "number";
    if (obj instanceof win.Date)
        return "date";
    if (obj instanceof win.RegExp)
        return "regexp";
    if (obj instanceof win.Error)
        return "error";
    return type;
}

Function.prototype.bind = function(thisObject)
{
    var func = this;
    var args = Array.prototype.slice.call(arguments, 1);
    return function() { return func.apply(thisObject, args.concat(Array.prototype.slice.call(arguments, 0))) };
}


String.prototype.format = function()
{
    var stringParts = this.split("%@");
    for (var i = 0; i < arguments.length; ++i)
        stringParts.splice(i * 2 + 1, 0, arguments[i].toString());
    return stringParts.join("");
}

Element.prototype.indexOfChildNode = function(childNode)
{
    var childNodes = this.childNodes;
    for (var i = 0; i < childNodes.length; ++i) {
        if (childNodes[i] === childNode)
            return i;
    }

    return -1;
}

Element.prototype.removeChildren = function()
{
    while (this.firstChild)
        this.removeChild(this.firstChild);
}

Element.prototype.__defineGetter__("totalOffsetLeft", function()
{
    var total = 0;
    for (var element = this; element; element = element.offsetParent)
        total += element.offsetLeft + (this !== element ? element.clientLeft : 0);
    return total;
});

Element.prototype.__defineGetter__("totalOffsetTop", function()
{
    var total = 0;
    for (var element = this; element; element = element.offsetParent)
        total += element.offsetTop + (this !== element ? element.clientTop : 0);
    return total;
});

Element.prototype.__defineGetter__("totalScrollTop", function()
{
    var total = 0;
    for (var element = this; element; element = element.offsetParent)
        total += element.scrollTop;
    return total;
});

Array.prototype.remove = function(value, onlyFirst)
{
    if (onlyFirst) {
        var index = this.indexOf(value);
        if (index !== -1)
            this.splice(index, 1);
        return;
    }

    var length = this.length;
    for (var i = 0; i < length; ++i) {
        if (this[i] === value)
            this.splice(i, 1);
    }
}

// @param element (HTML Element) The element whose total offset from left edge of the
//      page is desired.
// @return (integer) The total offset from the left edge of the page if the element is
//      not null or undefined or does not have a defined offsetLeft. -1 otherwise.
function totalLeftOffset(element)
{
    if (!element || typeof element.offsetLeft === "undefined")
        return -1;
    return element.totalOffsetLeft;
}

// @param element (HTML Element) The element whose total offset from top edge of the
//      page is desired.
// @return (integer) The total offset from the top edge of the page if the element is
//      not null or undefined or does not have a defined offsetTop. -1 otherwise.
function totalTopOffset(element)
{
    if (!element || typeof element.offsetTop === "undefined")
        return -1;
    return element.totalOffsetTop;
}

Node.prototype.enclosingNodeOrSelfWithNodeNameInArray = function(nameArray)
{
    for (var node = this; node && node !== this.ownerDocument; node = node.parentNode)
        for (var i = 0; i < nameArray.length; ++i)
            if (node.nodeName.toLowerCase() === nameArray[i].toLowerCase())
                return node;
    return null;
}

Node.prototype.enclosingNodeOrSelfWithNodeName = function(nodeName)
{
    return this.enclosingNodeOrSelfWithNodeNameInArray([nodeName]);
}

Number.constrain = function(num, min, max)
{
    if (num < min)
        num = min;
    else if (num > max)
        num = max;
    return num;
}

// JS class inheritance. NOTE: You should use the __proto__ approach instead of this.
var JSClass = {
    inherit: function(subclass, baseClass) {
        function inheritance() { }
        inheritance.prototype = baseClass.prototype;

        subclass.prototype = new inheritance();
        subclass.prototype.constructor = subclass;
        subclass.baseConstructor = baseClass;
        subclass.superClass = baseClass.prototype;
    }
}

// Dragging utilities ---------------------------------------------------------

var Direction = {
    HORIZONTAL: 0,
    VERTICAL: 1,
};

var Dragger = {
    // Sets up and starts the drag of an HTML element.
    //
    // @param element (HTML element) Element to drag.
    // @param elementDragging (function (event)) Function to be called while
    //     element is dragging.
    // @param elementDragEnd (function (event)) Function to be called when the
    //     drag ends.
    // @param event (event) The event that triggered the drag.
    // @param cursor (string) The cursor style to use while dragging.
    elementDragStart: function(element, elementDragging, elementDragEnd, event, cursor)
    {
        if (this._elementDraggingEventListener || this._elementEndDraggingEventListener)
            this.elementDragEnd(event);

        this._elementDraggingEventListener = elementDragging;
        this._elementEndDraggingEventListener = elementDragEnd;

        document.addEventListener("mousemove", elementDragging, true);
        document.addEventListener("mouseup", elementDragEnd, true);

        document.body.style.cursor = cursor;

        event.preventDefault();
    },
    
    // Finishes and tears down the drag of an element.
    // 
    // @param event (event) Event that causes the drag to end.
    elementDragEnd: function(event)
    {
        document.removeEventListener("mousemove", this._elementDraggingEventListener, true);
        document.removeEventListener("mouseup", this._elementEndDraggingEventListener, true);

        document.body.style.removeProperty("cursor");

        delete this._elementDraggingEventListener;
        delete this._elementEndDraggingEventListener;

        event.preventDefault();
    },
    
     // Updates the position of the element and all the elements around it.
     // If the direction of movement is horizontal (Dragger.HORIZONTAL), then A
     // is "left" and B is "right".
     // If the direction of movement is vertical (Dragger.VERTICAL), then A is
     // "above" and B is "below".
     //
     // @param element (HTML element) The element whose position to update.
     // @param direction (Direction.HORIZONTAL | Direction.VERTICAL) The direction
     //     in which the element is moving.
     // @param elementsToA (array of HTML elements) The elements to the A of the
     //     element.
     // @param elementsToB (array of HTML elements) The elements to the B of the
     //     element.
     // @param distanceToDragPoint (integer) The distance in pixels from the
     //     dragPoint to where the element should be placed.
     // @param minPos (integer) The minimum position the element can be at.
     // @param maxPos (integer) The maximum position the element can be at.
     // @param dragPoint (integer) The point at which the element has been
     //    dragged to.
    updateElementPosition: function(element, direction, elementsToA, elementsToB,
            distanceToDragPoint, minPos, maxPos, dragPoint)
    {
        if ((direction == Direction.HORIZONTAL && elementsToA[0].offsetWidth <= 0)
                || (elementsToA[0].offsetHeight <= 0)) {
            // The stylesheet hasn't loaded yet or the window is closed,
            // so we can't calculate what is need. Return early.
            return;
        }
        
        if (!("_currentElementPosition" in elementsToA[0])) {
            if (direction == Direction.HORIZONTAL)
                elementsToA[0]._currentElementPosition = elementsToA[0].offsetWidth;
            else
                elementsToA[0]._currentElementPosition = elementsToA[0].offsetHeight;
        }
        
        if (typeof dragPoint === "undefined")
            dragPoint = elementsToA[0]._currentElementPosition;
        
        dragPoint = Number.constrain(dragPoint, minPos, maxPos);
        elementsToA[0]._currentElementPosition = dragPoint;
        
        for (var i = 0; i < elementsToA.length; i++) {
            if (direction == Direction.HORIZONTAL)
                elementsToA[i].style.width = dragPoint + "px";
            else
                elementsToA[i].style.height = dragPoint + "px";
        }
        
        for (var j = 0; j < elementsToB.length; j++) {
            if (direction == Direction.HORIZONTAL)
                elementsToB[j].style.left = dragPoint + "px";
            else
                elementsToB[j].style.top = dragPoint + "px";
        }
        
        if (direction == Direction.HORIZONTAL)
            element.style.left = (dragPoint - distanceToDragPoint) + "px";
        else
            element.style.top = (dragPoint - distanceToDragPoint) + "px";
    }
};

// Listeners (taken from the WebInspector.Object in
// WebCore\inspector\frontend\Object.js)

Listeners = function() {
}

Listeners.prototype = {
    addEventListener: function(eventType, listener, thisObject) {
        if (!("_listeners" in this))
            this._listeners = {};
        if (!(eventType in this._listeners))
            this._listeners[eventType] = [];
        this._listeners[eventType].push({ thisObject: thisObject, listener: listener });
    },

    removeEventListener: function(eventType, listener, thisObject) {
        if (!("_listeners" in this) || !(eventType in this._listeners))
            return;
        var listeners = this._listeners[eventType];
        for (var i = 0; i < listeners.length; ++i) {
            if (listener && listeners[i].listener === listener && listeners[i].thisObject === thisObject)
                listeners.splice(i, 1);
            else if (!listener && thisObject && listeners[i].thisObject === thisObject)
                listeners.splice(i, 1);
        }

        if (!listeners.length)
            delete this._listeners[eventType];
    },

    dispatchEventToListeners: function(eventType) {
        if (!("_listeners" in this) || !(eventType in this._listeners))
            return;

        var stoppedPropagation = false;

        function stopPropagation()
        {
            stoppedPropagation = true;
        }

        function preventDefault()
        {
            this.defaultPrevented = true;
        }

        var event = {target: this, type: eventType, defaultPrevented: false};
        event.stopPropagation = stopPropagation.bind(event);
        event.preventDefault = preventDefault.bind(event);

        var listeners = this._listeners[eventType];
        for (var i = 0; i < listeners.length; ++i) {
            listeners[i].listener.call(listeners[i].thisObject, event);
            if (stoppedPropagation)
                break;
        }

        return event.defaultPrevented;
    }
}
