import React from 'react';
import ReactDOM from 'react-dom/client'; // Импортируем из react-dom/client
import App from './App';

const root = ReactDOM.createRoot(document.getElementById('root')); // Создаем корень
root.render(
    <React.StrictMode>
        <App />
    </React.StrictMode>
);