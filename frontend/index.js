fetch('/product')
    .then(response => response.json())
    .then(products => {
        const carousel = document.createElement('div');
        carousel.style.display = 'flex';
        carousel.style.overflowX = 'auto';
        carousel.style.gap = '16px';
        carousel.style.padding = '16px';

        products.forEach(product => {
            const card = document.createElement('div');
            card.style.minWidth = '200px';
            card.style.border = '1px solid #ccc';
            card.style.borderRadius = '8px';
            card.style.boxShadow = '0 2px 8px rgba(0,0,0,0.1)';
            card.style.padding = '16px';
            card.style.background = '#fff';
            card.style.display = 'flex';
            card.style.flexDirection = 'column';
            card.style.alignItems = 'center';

            // Example fields: name, price, image
            if (product.image) {
                const img = document.createElement('img');
                img.src = product.image;
                img.alt = product.name || 'Product';
                img.style.width = '100px';
                img.style.height = '100px';
                img.style.objectFit = 'cover';
                img.style.marginBottom = '8px';
                card.appendChild(img);
            }

            const name = document.createElement('h3');
            name.textContent = product.name || 'Unnamed Product';
            name.style.margin = '8px 0';
            card.appendChild(name);

            if (product.price) {
                const price = document.createElement('p');
                price.textContent = `$${product.price}`;
                price.style.fontWeight = 'bold';
                price.style.margin = '4px 0';
                card.appendChild(price);
            }

            carousel.appendChild(card);
        });

        document.body.appendChild(carousel);
    })
    .catch(error => {
        console.error('Error fetching products:', error);
    });