from node:latest

workdir /usr/src/app
copy package*.json ./
run npm install --only=production
copy . .

cmd node server.js
