

const client=await Deno.connect({port: 10000, transport: "tcp", hostname: "127.0.0.1"});

const clientId = 'ID_'+Math.random().toString(36).slice(4);

setInterval(()=>{
    client.write(new TextEncoder().encode(clientId+"=>ping"));
},1000)


while(1) {
    let buf=new Uint8Array(1024);
    const n=await client.read(buf) || 0;
    buf=buf.slice(0, n);
    console.log(clientId+' receive: >> ', new TextDecoder().decode(buf));
}
