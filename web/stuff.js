var size = 80;
const draw = SVG('board').size(size * 8, size * 8);

var white_pos = [{ x: 3, y: 3 }, { x: 4, y: 4 }];
var black_pos = [{ x: 3, y: 4 }, { x: 4, y: 3 }];
var valids = [{ x: 3, y: 2 }, { x: 2, y: 3 }, { x: 4, y: 5 }, { x: 5, y: 4 }]
var p1_turn = true;
function contains(a, obj) {
    for (var i = 0; i < a.length; i++) {
        if ((a[i].x === obj.x) && (a[i].y === obj.y)) {
            return true;
        }
    }
    return false;
}
function isValidMove(i, j) {
    console.log({ x: i, y: j })
    return contains(valids, { x: i, y: j })
}
function onTileClick(i, j) {
    return function () {
        if (isValidMove(i, j)) {
            var data = { 'x': i, 'y': j, 'w_pos': white_pos, 'b_pos': black_pos };
            var url = "http://127.0.0.1:5000/move";
            $.ajax({
                type: "POST",
                url: url,
                data: JSON.stringify(data),
                headers: { 'Content-type': 'application/json' },
                success: flaskCallback
            });
        } else {
            alert('INVALID MOVE!')
        }
    }
}
function flaskCallback(response) {
    var white = response.white;
    var black = response.black;
    white_pos = white;
    black_pos = black;
    valids = response.moves
    makeWhite(white)
    makeBlack(black)
    drawValid(valids)
    eg = response.eg;
    if (eg != 0) {
        if (white_pos.length>black_pos.length) {
            alert('White Wins! (' + white_pos.length.toString() + ' to ' +black_pos.length.toString() + ')')
        } else if (white_pos.length<black_pos.length) {
            alert('Black Wins! (' + black_pos.length.toString() + ' to ' +white_pos.length.toString()+ ')')
        }else{
            alert('Tie :/')
        }
    }
}
function makeWhite(coords) {
    for (var i = 0; i < coords.length; i++) {
        SVG.get('p_' + coords[i].x.toString() + coords[i].y.toString()).attr({ fill: '#fff', 'fill-opacity': '1.0' });
    }
}
function makeBlack(coords) {
    for (var i = 0; i < coords.length; i++) {
        SVG.get('p_' + coords[i].x.toString() + coords[i].y.toString()).attr({ fill: '#000', 'fill-opacity': '1.0' });
    }
}
function drawValid(coords) {
    SVG.select('circle.move-marker').attr({ 'fill': '#000', 'fill-opacity': '0.0' });
    for (var i = 0; i < coords.length; i++) {
        SVG.get('v_' + coords[i].x.toString() + coords[i].y.toString()).attr({ fill: '#00f', 'fill-opacity': '1.0' });
    }
}
function drawBoard(pieces) {
    for (var i = 0; i < 8; i++) {
        for (var j = 0; j < 8; j++) {
            var scale = 0.8;
            var vscale = 0.1;
            draw.rect(size, size).move(i * size, j * size).attr({ fill: '#080', stroke: 'black' }).attr('class', 'back');
            draw.circle(scale * size).move((i + ((1 - scale) / 2)) * size, (j + ((1 - scale) / 2)) * size).id('p_' + i.toString() + j.toString()).attr({ 'fill': '#000', 'fill-opacity': '0.0' }).attr('class', 'piece');
            draw.circle(vscale * size).move((i + ((1 - vscale) / 2)) * size, (j + ((1 - vscale) / 2)) * size).id('v_' + i.toString() + j.toString()).attr({ 'fill': '#000', 'fill-opacity': '0.0' }).attr('class', 'move-marker');
            draw.rect(size, size).move(i * size, j * size).id('sq_' + i.toString() + j.toString()).attr({ 'fill': '#000', 'fill-opacity': '0.0' }).on('click', onTileClick(i, j)).attr('class', 'button');
        }
    }
    makeWhite(white_pos);
    makeBlack(black_pos);
    drawValid(valids);
}
drawBoard();
