const listener2 = Deno.listen({ port: 10000, transport: 'tcp', hostname: '127.0.0.1' });
let count = 0;

for await (const conn of listener2) {
    count++;
    console.log('client count:' + count);
    handleConn(conn);
}

async function handleConn(conn: Deno.Conn) {
    while (1) {
        let buf = new Uint8Array(50);
        const n = (await conn.read(buf)) || 0;
        buf = buf.slice(0, n);
        const str = new TextDecoder().decode(buf);
        console.log('Server receive: ', str);
        const msg = str + '====>reply pong';
        await conn.write(new TextEncoder().encode(msg));
    }
}
