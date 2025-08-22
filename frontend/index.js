fetch('/product')
    .then(response => response.json())
    .then(products => {
        const div = document.createElement('div');
        div.textContent = JSON.stringify(products, null, 2);
        document.body.appendChild(div);
    })
    .catch(error => {
        console.error('Error fetching products:', error);
    });