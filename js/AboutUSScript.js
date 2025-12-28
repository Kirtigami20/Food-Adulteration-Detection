document.addEventListener("DOMContentLoaded", function () {
    const members = [
        { name: "Kirti" },
        { name: "Sana" },
        { name: "Viren" },
        { name: "Viya" },

    ];

    members.forEach((member, index) => {
        const frame = document.getElementById(`member${index + 1}`);
        if (frame) {
            frame.querySelector("h4").innerText = member.name;
        }
    });

    const frames = document.querySelectorAll(".frame");

    frames.forEach((frame) => {
        frame.addEventListener("mouseenter", () => {
            frames.forEach((otherFrame) => {
                if (otherFrame !== frame) {
                    otherFrame.classList.add("blur");
                }
            });
        });

        frame.addEventListener("mouseleave", () => {
            frames.forEach((otherFrame) => {
                otherFrame.classList.remove("blur");
            });
        });
    });
    
    // Automatic Carousel Sliding
    const slider = document.querySelector('.slider');
    let currentSlide = 0;
    const slides = slider.children;
    const totalSlides = slides.length;

    setInterval(() => {
        currentSlide = (currentSlide + 1) % totalSlides;
        slider.scrollTo({
            left: currentSlide * slider.clientWidth,
            behavior: 'smooth'
        });
    }, 2000);
});
