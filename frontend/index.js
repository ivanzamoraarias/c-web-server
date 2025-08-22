let targetX = 0;
let targetY = 0;
let currentX = 0;
let currentY = 0;
const speed = 10; // pixels per frame

function setNewTarget() {
    const maxX = window.innerWidth - 80;
    const maxY = window.innerHeight - 70;
    targetX = Math.random() * maxX;
    targetY = Math.random() * maxY;
}

function animateTriangle() {
    const dx = targetX - currentX;
    const dy = targetY - currentY;
    const distance = Math.sqrt(dx * dx + dy * dy);

    if (distance < speed) {
        currentX = targetX;
        currentY = targetY;
        setNewTarget();
    } else {
        currentX += (dx / distance) * speed;
        currentY += (dy / distance) * speed;
    }
    triangle.style.left = `${currentX}px`;
    triangle.style.top = `${currentY}px`;
    requestAnimationFrame(animateTriangle);
}

document.addEventListener('DOMContentLoaded', () => {
    setNewTarget();
    animateTriangle();
});
document.addEventListener('DOMContentLoaded', () => {
    const triangle = document.createElement('div');
    triangle.style.position = 'fixed';
    triangle.style.width = '0';
    triangle.style.height = '0';
    triangle.style.borderLeft = '40px solid transparent';
    triangle.style.borderRight = '40px solid transparent';
    triangle.style.borderBottom = '70px solid #3498db';
    triangle.style.zIndex = '9999';
    document.body.appendChild(triangle);

    function moveTriangle() {
        const maxX = window.innerWidth - 80; // triangle width
        const maxY = window.innerHeight - 70; // triangle height
        const x = Math.random() * maxX;
        const y = Math.random() * maxY;
        triangle.style.left = `${x}px`;
        triangle.style.top = `${y}px`;
    }

    //setInterval(moveTriangle, 1000);
    //moveTriangle();
});