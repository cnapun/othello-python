from flask import Flask, jsonify, request
from flask_cors import CORS

from BB import *

l = Learner(4)
l.load_weights('weights1_100')
bb = Bitboard(4, l.heuristic)

app = Flask(__name__)
CORS(app)

class IllegalMove(Exception):
    status_code=400

    def __init__(self, message, status_code=None, payload=None):
        Exception.__init__(self)
        self.message = message
        self.status_code = status_code or self.status_code
        self.payload = payload
    def to_dict(self):
        rv = dict(self.payload or ())
        rv['message'] = self.message
        return rv

@app.errorhandler(IllegalMove)
def handle_bad_move(error):
    response = jsonify(error.to_dict())
    response.status_code = error.status_code
    return response


@app.route('/move', methods=['POST'])
def get_task():
    data = request.get_json()
    w = get_bb_from_list(data['w_pos'])
    b = get_bb_from_list(data['b_pos'])
    mv = get_bb(data['x'], data['y'])
    bb.p1 = b
    bb.p2 = w
    bb.p1turn = True
    try:
        eg = bb.make_move(mv)
    except:
        raise IllegalMove(str(mv))
    while (not bb.p1turn) and eg == 0:
        eg = bb.make_move(bb.best_move(False))
        if eg==2:
            eg = 1
        elif eg==1:
            eg = 2
    p1 = bb.to_array(bb.p1).reshape(8,8)
    p2 = bb.to_array(bb.p2).reshape(8,8)
    moves = bb.to_array(bb.moves()).reshape(8,8)
    response = jsonify({'white':[{'x':int(j), 'y':int(i)} for i,j in zip(*np.where(p2!=0))], 'black':[{'x':int(j), 'y':int(i)} for i,j in zip(*np.where(p1!=0))], 'eg':eg, 'moves':[{'x':int(j), 'y':int(i)} for i,j in zip(*np.where(moves!=0))]})
    response.status_code = 200
    # print(data)
    return response

if __name__ == '__main__':
    app.run(debug=True)