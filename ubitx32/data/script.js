//------------------------------------------------------
//   WebSocket con reconexión automática inteligente
//------------------------------------------------------


let ws = null;
let reconnectTimer = null;
let reconnectDelay = 2000; // 2 segundos
let messageQueue = [];     // cola de mensajes cuando WS no está listo


function conectarWS() {
    const url = `ws://${location.hostname}:81`;
    console.log("Conectando a WebSocket:", url);

    ws = new WebSocket(url);

    ws.onopen = () => {
        console.log("WebSocket conectado.");
        document.getElementById("estado").innerText = "Conectado";

        // Enviar mensajes pendientes
        if (messageQueue.length > 0) {
            console.log("Enviando mensajes pendientes...");
            messageQueue.forEach(msg => ws.send(msg));
            messageQueue = [];
        }

        if (reconnectTimer) {
            clearTimeout(reconnectTimer);
            reconnectTimer = null;
        }
    };

    ws.onerror = (err) => {
        console.log("Error en WebSocket:", err);
        document.getElementById("estado").innerText = "Error de conexión";
    };

    ws.onclose = () => {
        console.log("WebSocket cerrado");
        document.getElementById("estado").innerText = "Desconectado – Reintentando...";

        if (!reconnectTimer) {
            reconnectTimer = setTimeout(() => {
                conectarWS();
            }, reconnectDelay);
        }
    };

    ws.onmessage = (event) => {
        procesarMensaje(event);
    };
}

// Iniciar conexión
document.addEventListener("DOMContentLoaded", () => {
    conectarWS();
});


//------------------------------------------------------
//   Enviar mensajes con seguridad (cola si WS no abierto)
//------------------------------------------------------
function enviar(tipo, valor) {
    const mensaje = JSON.stringify({ t: tipo, v: valor });

    if (ws.readyState === WebSocket.OPEN) {
        ws.send(mensaje);
    } else {
        console.warn("WS no abierto, mensaje en cola:", mensaje);
        messageQueue.push(mensaje);
    }
}

//------------------------------------------------------
//   PROCESAR MENSAJES DEL SERVIDOR (tu código original)
//------------------------------------------------------
function procesarMensaje(event) {
    const data = JSON.parse(event.data);
    const tipo = data.t;
    const valor = data.v;
//    console.log("Tipo:", tipo);
//    console.log("Valor:",  valor);

    if (tipo === "date") {
        document.getElementById("fechaHora").textContent = valor;
        return;
    }

    if (tipo === "temp") {
        const temp1 = (parseInt(valor.temp1) / 100).toFixed(1);
        const temp2 = (parseInt(valor.temp2) / 100).toFixed(1);
        const temp3 = (parseInt(valor.temp3) / 100).toFixed(1);

        if (document.getElementById("temp1")) document.getElementById("temp1").innerText = temp1 + " °C";
        if (document.getElementById("temp2")) document.getElementById("temp2").innerText = temp2 + " °C";
        if (document.getElementById("temp3")) document.getElementById("temp3").innerText = temp3 + " °C";
        return;
    }

    if (tipo === "s") {
        const sMeterValue = parseInt(valor); // usar valor directamente
        moverAgujaSMeter(sMeterValue);
        return;
    }

 if (tipo === "tx") {
        const pwrValue = parseInt(valor.p);
        const swrValue = parseFloat(valor.swr);

        moverAgujaPwr(pwrValue);

        const swrElement = document.getElementById("swr");
        if (swrElement) swrElement.innerText = swrValue.toFixed(1);
        return;
    }

    const el = document.getElementById(tipo);
    if (el) el.innerText = valor;

    if (tipo === "temp1" || tipo === "temp2" || tipo === "temp3") {
        const tempElement = document.getElementById(tipo);
        if (tempElement) tempElement.innerText = valor + " °C";
        return;
    }

 // --- Actualizar botones de modo según mensajes del servidor ---
    if (tipo === "ssb") {
        const btnSSB = document.getElementById("btnSSB");
        if (btnSSB) {
            btnSSB.textContent = (valor == 1) ? "USB" : "LSB";
            btnSSB.style.backgroundColor = (valor == 1) ? "yellow" : "#3498db";
            btnSSB.style.color = (valor == 1) ? "black" : "white";
        }
    }

    if (tipo === "cw") {
        const btnCW = document.getElementById("btnCW");
        if (botonVfo) {
            btnCW.style.backgroundColor = (valor == 1) ? "yellow" : "#3498db";
            btnCW.style.color = (valor == 1) ? "black" : "white";
        return;
        }
    }


    if (tipo === "vfo") {
        const botonVfo = document.getElementById("botonVfo");
        if (botonVfo) {
            botonVfo.innerText = valor === "A" ? "VFO A" : "VFO B";
            botonVfo.style.backgroundColor = (valor === "A") ? "green" : "#3498db";
        return;
        }
    }

    if (tipo === "spl") {
        const botonSplit = document.getElementById("botonSpl");
        if (botonSplit) {
            botonSplit.innerText = valor === "ON" ? "SPLIT ON" : "SPLIT OFF";
            botonSplit.style.backgroundColor = (valor === "ON") ? "orange" : "gray";
        return;
        }
    }

    if (tipo === "rit") {
        const botonRit = document.getElementById("botonRit");
        if (botonRit) {
            botonRit.innerText = valor === "ON" ? "RIT ON" : "RIT OFF";
            botonRit.style.backgroundColor = (valor === "ON") ? "#3498db" : "gray";
        return;
        }
    }

    if (tipo === "fA") {
        let frecuencia = valor.toString().padStart(9, '0');

        const digits = document.querySelectorAll('#fA_digits .digit');
        digits.forEach((digit, index) => {
            digit.innerText = frecuencia.charAt(index);
        });

        const fAElement = document.getElementById('fA');
        if (fAElement) fAElement.innerText = frecuencia;

        frecuenciaA = frecuencia;
        return;
    }
    if (tipo === "vt") {
        const voltElement = document.getElementById("volt");
        if (voltElement) {
            voltElement.innerText = valor + " V";
        return;
        }
    }
    if (tipo === "it") {
        const intElement = document.getElementById("curr");
        if (intElement) {
            intElement.innerText = valor + " A";
        return;
        }
    }

}


//------------------------------------------------------
//   AGUJAS (igual que lo tenías)
//------------------------------------------------------
function moverAgujaPwr(valor) {
    const aguja = document.getElementById("agujaPwr");
    if (!aguja) return;

    const valorMaximo = 100;
    const anguloMin = -90;
    const anguloMax = 90;

    const porcentaje = Math.min(Math.max(valor / valorMaximo, 0), 1);
    const angulo = anguloMin + porcentaje * (anguloMax - anguloMin);

    aguja.style.transform = `rotate(${angulo}deg)`;
}

function moverAgujaSMeter(valor) {
    const aguja = document.getElementById("agujaSMeter");
    if (!aguja) return;

    const angulo = (valor / 100) * 100 - 50;
    aguja.style.transform = `rotate(${angulo}deg)`;
    aguja.style.transformOrigin = "bottom center";
}


//------------------------------------------------------
//   DÍGITOS DEL VFO A (igual que lo tenías)
//------------------------------------------------------
const digits = document.querySelectorAll("#fA_digits .digit");
let frecuenciaA = "000000000";

function handleWheelEvent(event) {
    event.preventDefault();

    const target = event.target;
    if (!target.classList.contains("digit")) return;

    const index = parseInt(target.dataset.index, 10);
    let digitValue = parseInt(target.innerText, 10);

    if (event.deltaY < 0) digitValue = (digitValue + 1) % 10;
    else if (event.deltaY > 0) digitValue = (digitValue - 1 + 10) % 10;

    target.innerText = digitValue;

    frecuenciaA =
        frecuenciaA.substring(0, index) +
        digitValue +
        frecuenciaA.substring(index + 1);

    frecuenciaA = frecuenciaA.padStart(9, '0');

    enviar('fA', frecuenciaA);
}

digits.forEach(digit => {
    digit.addEventListener("wheel", handleWheelEvent);
});


