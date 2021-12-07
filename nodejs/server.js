const net = require("net");
// Creating and initializing server
const server = net.createServer();

let count =0;

server.on("connection", (connection) => {
  connection.write("ok.");

  count++;
  console.log('client count:'+count)
  setInterval(() => {
    connection.write("====>push message to client");
  }, 5000);

  connection.on("data", (data) => {
    console.log(`${Date.now()} Received client data: ${data.toString()}`);
    connection.write("server pong ===>"); // echo received data back
  });

  connection.on("close", () => {
    console.log("connection closed");
  });

  connection.on("end", () => {
    console.log("connection end");
  });
});

server.listen(4000);

console.log("tcp server listen 4000");
