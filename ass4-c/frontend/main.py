from flask import Flask,jsonify,request,render_template
import sys
import ctypes
import subprocess
import os


app = Flask(__name__)


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/build')
def build():
    return render_template('build.html')


@app.route('/rbuild', methods=['POST'])
def rbuild():
    rid = request.form.get('build_method')
    if rid == "build":
        command = "controller.exe j null"
    else:
        command = "controller.exe r null"

    myCmd = os.popen(command).read()
    # Further file processing goes here
    return render_template(
        'build_result.html',
        result = myCmd
    )


@app.route('/search')
def search():
    return render_template('search.html')

@app.route('/rsearch', methods=['POST'])
def rsearch():
    rid = request.form.get('record')
    command = "controller.exe s " + rid

    myCmd = os.popen(command).read()
    fp = open('./server/tmp.txt', 'r')
    result = fp.readlines()
    fp.close()

    fp = open('./server/info.txt', 'r')
    runtime = fp.readlines()
    fp.close()
    # with open('./server/info.txt', "r") as fp2:
    #     runtime = fp.readlines()

    # Further file processing goes here
    return render_template(
        'search_result.html',
        rid = rid,
        runtime = runtime,
        result = result,
        title = result[1][5:],
        content = result[2][9:],
        viewCount = result[3],
        res = result[4],
        duration = result[5]
    )
# #post /store data: {name :}
# @app.route('/store' , methods=['POST'])
# def create_store():
#   request_data = request.get_json()
#   new_store = {
#     'name':request_data['name'],
#     'items':[]
#   }
#   stores.append(new_store)
#   return jsonify(new_store)
#   #pass
#
# #get /store/<name> data: {name :}
# @app.route('/store/<string:name>')
# def get_store(name):
#   for store in stores:
#     if store['name'] == name:
#           return jsonify(store)
#   return jsonify ({'message': 'store not found'})
#   #pass
#
# #get /store
# @app.route('/store')
# def get_stores():
#   return jsonify({'stores': stores})
#   #pass
#
# #post /store/<name> data: {name :}
# @app.route('/store/<string:name>/item' , methods=['POST'])
# def create_item_in_store(name):
#   request_data = request.get_json()
#   for store in stores:
#     if store['name'] == name:
#         new_item = {
#             'name': request_data['name'],
#             'price': request_data['price']
#         }
#         store['items'].append(new_item)
#         return jsonify(new_item)
#   return jsonify ({'message' :'store not found'})
#   #pass
#
# #get /store/<name>/item data: {name :}
# @app.route('/store/<string:name>/item')
# def get_item_in_store(name):
#   for store in stores:
#     if store['name'] == name:
#         return jsonify( {'items':store['items'] } )
#   return jsonify ({'message':'store not found'})
#
#   #pass
app.debug = True
app.run(port=5000)
